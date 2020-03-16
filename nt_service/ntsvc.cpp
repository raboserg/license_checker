#if defined(ACE_WIN32) && !defined(ACE_LACKS_WIN32_SERVICES)

#include "ntsvc.h"
#include "ace/Date_Time.h"
#include "config_change.h"
#include "parser_ini.h"
#include "tracer.h"

Service::Service(void)
    : event_(std::make_shared<ACE_Auto_Event>()),
      done_handler_(ACE_Sig_Handler_Ex(ACE_Reactor::end_event_loop)) {
  this->svc_status_.dwServiceType =
      SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS;
  // Remember the Reactor instance.
  reactor(ACE_Reactor::instance());
}

Service::~Service(void) {
  /*if (this->reactor()->cancel_timer(this) == -1)
    ACE_ERROR(
        (LM_ERROR, "%T (%t):\tService::~Service failed to cancel_timer.\n"));
  DEBUG_LOG(TM("Service::~Service failed to cancel_timer"));*/
}

int Service::handle_close(ACE_HANDLE, ACE_Reactor_Mask) {
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T \tService::handle_close \t (%t) \n")));
  stop_ = 1;
  reactor()->end_reactor_event_loop();
  // ACE_Reactor::end_event_loop
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

int Service::reshedule_tasks() {
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T Service: reshedule_tasks (%t) \n")));

  const Options options = PARSER::instance()->options();
  get_license_task_->set_day_waiting_hours(options.next_day_waiting_hours);
  get_license_task_->set_try_get_license_mins(
      options.next_try_get_license_mins);
  get_license_task_->schedule_handle_timeout(5);

  process_killer_task_->process_stopping_name(options.kill_file_name);
  process_killer_task_->set_day_waiting_hours(options.next_day_waiting_hours);
  process_killer_task_->schedule_handle_timeout(5);

  // reactor()->schedule_timer(get_license_task_, 0, tv1, ACE_Time_Value::zero);
  return 0;
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
  return -1;
}

// Beep every two seconds.  This is what this NT service does...
int Service::handle_timeout(const ACE_Time_Value &tv, const void *) {
  ACE_UNUSED_ARG(tv);
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T (%t):\thandle timeout...\n")));
  return 0;
}

// This is the main processing function for the Service.  It sets up
// the initial configuration and runs the event loop until a close
// request is received.
int Service::svc(void) {

  ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T \tStart Service::svc \t (%t) \n")));
  DEBUG_LOG(TM("Start Service::svc"));

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
  if (PARSER::instance()->init() == -1) {
    ACE_ERROR((LM_ERROR, ACE_TEXT("%T \tCannot to initialize constants\n")));
    ACE_ERROR((LM_SHUTDOWN, ACE_TEXT("%T \tShutting down service (%t) \n")));
    INFO_LOG(TM("Shutting down service"));
    ACE_OS::sleep(2);
    raise(SIGINT);
  }

  const Options options = PARSER::instance()->options();

  const std::shared_ptr<WinNT::Notificator> notificator_ =
      std::make_shared<WinNT::Notificator>();
  if (notificator_->Initialize(this->event_) == -1) {
    ACE_ERROR((LM_ERROR,
               "%T (%t) \tCannot to initialize notificator for event sink\n"));
    reactor()->notify(this, ACE_Event_Handler::EXCEPT_MASK);
  }

  if (this->reactor()->register_handler(this, event_->handle()) == -1) {
    ACE_ERROR((LM_ERROR, "%T (%t) \tcannot to register handle with Reactor\n",
               "Service::svc"));
    reactor()->notify(this, ACE_Event_Handler::EXCEPT_MASK);
  }

  // Handle signals through the ACE_Reactor.
  if (this->reactor()->register_handler(SIGINT, &this->done_handler_) == -1) {
    ACE_ERROR((LM_ERROR, "%T \tcannot to register_handler SIGINT (%t) \n"));
    reactor()->notify(this, ACE_Event_Handler::EXCEPT_MASK);
  }

  const int waiting_mins = options.next_try_get_license_mins;
  const int waiting_hours = options.next_day_waiting_hours;
  get_license_task_ =
      std::make_unique<Get_License_Task>(waiting_mins, waiting_hours);
  if (get_license_task_->open(ACE_Time_Value(5)) == -1) {
    ACE_ERROR((LM_ERROR, "%T \tcannot to open get_license_task (%t)\n"));
    reactor()->notify(this, ACE_Event_Handler::EXCEPT_MASK);
  }

  process_killer_task_ = std::make_unique<Process_Killer_Task>();
  process_killer_task_->set_day_waiting_hours(waiting_hours);
  process_killer_task_->process_stopping_name(options.kill_file_name);
  if (process_killer_task_->open(ACE_Time_Value(5, 0)) == -1) {
    ACE_ERROR((LM_ERROR, "%T \tcannot to open process_killer_task\t (%t) \n"));
    reactor()->notify(this, ACE_Event_Handler::EXCEPT_MASK);
  }

  const std::unique_ptr<Config_Handler> config_handler =
      std::make_unique<Config_Handler>(ACE_Reactor::instance());

  this->reactor()->run_event_loop();
  // this->msg_queue();
  // Cleanly terminate connections, terminate threads.
  ACE_DEBUG((LM_SHUTDOWN, ACE_TEXT("%T Shutting down service (%t) \n")));
  INFO_LOG(TM("Shutting down service"));
  notificator_->Release();

  // this->reactor()->cancel_timer(this);
  this->reactor()->remove_handler(this->event_->handle(),
                                  ACE_Event_Handler::DONT_CALL);
  ACE_OS::sleep(3);
  return 0;
}

#endif /* ACE_WIN32 && !ACE_LACKS_WIN32_SERVICES */
