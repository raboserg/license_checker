#include "ntsvc.h"
#include "ace/Date_Time.h"
#include "parser_ini.h"
#include "tracer.h"

#if defined(ACE_WIN32) && !defined(ACE_LACKS_WIN32_SERVICES)
/*,done_handler_(ACE_Sig_Handler_Ex(ACE_Reactor::end_event_loop))*/
Service::Service(void)
    : event_(std::make_shared<ACE_Auto_Event>()),
      done_handler_(ACE_Sig_Handler_Ex(ACE_Reactor::end_event_loop)) {
  this->svc_status_.dwServiceType =
      SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS;
  // Remember the Reactor instance.
  reactor(ACE_Reactor::instance());
  DEBUG_LOG(TM("Service::Service(void) "));
}

Service::~Service(void) {
  if (ACE_Reactor::instance()->cancel_timer(this) == -1)
    ACE_ERROR(
        (LM_ERROR, "%T (%t):\tService::~Service failed to cancel_timer.\n"));
  DEBUG_LOG(TM("Service::~Service failed to cancel_timer"));
}

int Service::handle_close(ACE_HANDLE, ACE_Reactor_Mask) {
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T (%t):\tService::handle_close \n")));
  stop_ = 1;
  reactor()->end_reactor_event_loop();
  return 0;
}
// This method is called when the service gets a control request.  It
// handles requests for stop and close by calling terminate ().
// All others get handled by calling up to inherited::handle_control.
void Service::handle_control(DWORD control_code) {
  if (control_code == SERVICE_CONTROL_SHUTDOWN ||
      control_code == SERVICE_CONTROL_STOP) {
    report_status(SERVICE_STOP_PENDING);
    ACE_DEBUG(
        (LM_INFO, ACE_TEXT("%T (%t):\tService control stop requested\n")));
    DEBUG_LOG(TM("Service control stop requested"));
    reactor()->notify(this, ACE_Event_Handler::EXCEPT_MASK);
    //???reactor()->notify(this->get_license_task_,
    // ACE_Event_Handler::EXCEPT_MASK);
  } else
    inherited::handle_control(control_code);
}

int Service::handle_signal(int, siginfo_t *siginfo, ucontext_t *) {
  DEBUG_LOG(TM("Service::handle_signal..."));
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T (%t):\tService::handle_signal...\n")));

  return 0;
}

// This is just here to be the target of the notify from above... it
// doesn't do anything except aid on popping the reactor off its wait
// and causing a drop out of handle_events.
int Service::handle_exception(ACE_HANDLE) {
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T (%t):\tService::handle_exception()\n")));
  //???  DEBUG_LOG(TM("int Service::handle_exception(ACE_HANDLE)"));
  return -1;
}

// Beep every two seconds.  This is what this NT service does...
int Service::handle_timeout(const ACE_Time_Value &tv, const void *) {
  ACE_UNUSED_ARG(tv);
  // TRACE_LOG(TM("handle timeout..."));
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T (%t):\thandle timeout...\n")));
  return 0;
}

// This is the main processing function for the Service.  It sets up
// the initial configuration and runs the event loop until a close
// request is received.
int Service::svc(void) {

  ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T (%t):\tService::svc\n")));
  DEBUG_LOG(TM("Service::svc"));

  // As an NT service, we come in here in a different thread than the
  // one which created the reactor.  So in order to do anything, we
  // need to own the reactor. If we are not a service, report_status
  // will return -1.
  if (report_status(SERVICE_RUNNING) == 0)
    reactor()->owner(ACE_Thread::self());

  // Schedule a timer every two seconds.
  ACE_Time_Value tv(2, 0);
  // ACE_Reactor::instance()->schedule_wait(this, 0, tv, tv);
  //////////////////////////////////////////////////////////////////////////

  int arg1 = 1;
  CB cb1(arg1);
  MyActiveTimer atimer;
  const ACE_Time_Value curr_tv = ACE_OS::gettimeofday();
  ACE_Time_Value interval = ACE_Time_Value(1, 100000);
  // atimer.schedule(&cb1, &arg1, curr_tv + ACE_Time_Value(3L), interval);

  ////////////////////////////////////////////////////////////////////////
  if (PARSER::instance()->init() == -1){
    ACE_ERROR((LM_ERROR, "%T (%t):\t) \n", "Service::svc"));
  // raise(SIGINT);
    }

  notificator_ = std::make_shared<WinNT::Notificator>();
  if (this->notificator_->Initialize(this->event_) == -1) {
    ACE_ERROR(
        (LM_ERROR, "%T (%t):\tcannot initialize notificator for event sink\n"));
    reactor()->notify(this, ACE_Event_Handler::EXCEPT_MASK);
  }

  if (this->reactor()->register_handler(this, event_->handle()) == -1) {
    ACE_ERROR((LM_ERROR, "%T (%t):\tcannot register handle with Reactor\n",
               "Service::svc"));
    ERROR_LOG(TM("cannot register handle with Reactor"));
  }

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("%T (%t):\tif (this->reactor()->register_handler(this, "
                      "event_->handle()) == -1) {;\n")));

  // Handle signals through the ACE_Reactor.
  if (this->reactor()->register_handler(SIGINT, &this->done_handler_) == -1)
    ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("register_handler")),
                     -1);

  get_license_task_ = std::make_unique<Get_License_Task>();
  if (this->get_license_task_->open(ACE_Time_Value(5)) == -1) {
    ACE_ERROR((LM_ERROR, "%T (%t):\tcannot open get_license_task \n"));
    reactor()->notify(this, ACE_Event_Handler::EXCEPT_MASK);
  }

  process_killer_task_ = std::make_unique<Process_Killer_Task>();
  const string_t process_stopping_name =
      PARSER::instance()->options().kill_file_name;
  process_killer_task_->process_stopping_name(process_stopping_name);
  if (this->process_killer_task_->open(ACE_Time_Value(5, 0)) == -1) {
    ACE_ERROR((LM_ERROR, "%T (%t):\tcannot open get_license_task \n"));
    reactor()->notify(this, ACE_Event_Handler::EXCEPT_MASK);
  }

  this->reactor()->run_reactor_event_loop();

  // this->msg_queue();

  // Cleanly terminate connections, terminate threads.
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T (%t):\tShutting down\n")));
  this->notificator_->Release();

  this->get_license_task_->close();
  this->process_killer_task_->close();

  this->reactor()->cancel_timer(this);
  this->reactor()->remove_handler(this->event_->handle(),
                                  ACE_Event_Handler::DONT_CALL);

  return 0;
}

#endif /* ACE_WIN32 && !ACE_LACKS_WIN32_SERVICES */
