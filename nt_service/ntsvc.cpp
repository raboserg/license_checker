#include "ntsvc.h"
#include "tracer.h"

#include "ace/Date_Time.h"

#if defined(ACE_WIN32) && !defined(ACE_LACKS_WIN32_SERVICES)

Service::Service(void)
    : event_(std::make_shared<ACE_Auto_Event>()), get_task_(new Get_License_Task()) {
  this->svc_status_.dwServiceType =
      SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS;
  // Remember the Reactor instance.
  reactor(ACE_Reactor::instance());
  notificator_ = std::make_shared<WinNT::Notificator>();
  DEBUG_LOG(TM("Service::Service(void) "));
}

Service::~Service(void) {
  if (ACE_Reactor::instance()->cancel_timer(this) == -1)
    ACE_ERROR((LM_ERROR, "Service::~Service failed to cancel_timer.\n"));
	if(ACE_Reactor::instance()->cancel_timer(get_task_) == -1)
		ACE_ERROR((LM_ERROR, "Service::~Service failed to cancel_timer.\n"));
  delete get_task_;
  DEBUG_LOG(TM("Service::~Service failed to cancel_timer"));
}

int Service::handle_close(ACE_HANDLE, ACE_Reactor_Mask) {
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T (%t): Service::handle_close #####\n")));
  reactor()->end_reactor_event_loop();
  return 0;
}
// This method is called when the service gets a control request.  It
// handles requests for stop and shutdown by calling terminate ().
// All others get handled by calling up to inherited::handle_control.
void Service::handle_control(DWORD control_code) {
  if (control_code == SERVICE_CONTROL_SHUTDOWN ||
      control_code == SERVICE_CONTROL_STOP) {
    report_status(SERVICE_STOP_PENDING);
    ACE_DEBUG((LM_INFO, ACE_TEXT("Service control stop requested\n")));
    DEBUG_LOG(TM("Service control stop requested"));
    stop_ = 1;
    reactor()->notify(this, ACE_Event_Handler::EXCEPT_MASK);
    //???reactor()->notify(this->get_task_, ACE_Event_Handler::EXCEPT_MASK);
  } else
    inherited::handle_control(control_code);
}

int Service::handle_signal(int, siginfo_t *siginfo, ucontext_t *) {
  DEBUG_LOG(TM("Service::handle_signal..."));
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T (%t): Service::handle_signal...\n")));
  return 0;
}

// This is just here to be the target of the notify from above... it
// doesn't do anything except aid on popping the reactor off its wait
// and causing a drop out of handle_events.
int Service::handle_exception(ACE_HANDLE) {
  ACE_DEBUG(
      (LM_DEBUG, ACE_TEXT("int Service::handle_exception(ACE_HANDLE)\n")));
  DEBUG_LOG(TM("int Service::handle_exception(ACE_HANDLE)"));
  return -1;
}

// Beep every two seconds.  This is what this NT service does...
int Service::handle_timeout(const ACE_Time_Value &tv, const void *) {
  ACE_UNUSED_ARG(tv);
	//TRACE_LOG(TM("handle timeout..."));
	//ACE_TEXT(" sent=%d,recv=%d,skip=%d\n"),
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T (%t): handle timeout...\n")));
  return 0;
}

// This is the main processing function for the Service.  It sets up
// the initial configuration and runs the event loop until a shutdown
// request is received.
int Service::svc(void) {
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("Service::svc\n")));
  DEBUG_LOG(TM("Service::svc"));

  // As an NT service, we come in here in a different thread than the
  // one which created the reactor.  So in order to do anything, we
  // need to own the reactor. If we are not a service, report_status
  // will return -1.
  if (report_status(SERVICE_RUNNING) == 0)
    reactor()->owner(ACE_Thread::self());

  ACE_Time_Value tv1(5, 0);
  ACE_Reactor::instance()->schedule_timer(get_task_, 0, tv1, tv1);

	////////////////////////////////////////////////////////////////////////
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T (%t): ACE_OS::gettimeofday()=%d\n"), ACE_OS::gettimeofday()));
	ACE_Date_Time date_time;
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("ACE_Date_Time - Date %t %d\n"), date_time.day()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("ACE_Date_Time - Month %t %d\n"), date_time.month()));
		
	int arg1 = 1;
	CB cb1(arg1);
	MyActiveTimer atimer;
	const ACE_Time_Value curr_tv = ACE_OS::gettimeofday();
	ACE_Time_Value interval = ACE_Time_Value(1, 100000);
	atimer.schedule(&cb1, &arg1, curr_tv + ACE_Time_Value(3L), interval);
	////////////////////////////////////////////////////////////////////////
		
  if (this->reactor()->register_handler(this, event_->handle()) == -1) {
    ACE_ERROR((LM_ERROR, "%p\t cannot register handle with Reactor\n",
               "Service::svc"));
    ERROR_LOG(TM("cannot register handle with Reactor"));
  }
  // Schedule a timer every two seconds.
  ACE_Time_Value tv(2, 0);
  ACE_Reactor::instance()->schedule_timer(this, 0, tv, tv);

  notificator_->Initialize(this->event_);
	//???Auto_Destroyer<ACE_URL_Addr> url_addr_ptr(url_addr);


	//this->stop_ = 0;
  // while (!this->stop_)reactor()->handle_events();

  reactor()->run_reactor_event_loop();

  notificator_->Release();

  // Cleanly terminate connections, terminate threads.
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("Shutting down\n")));

  reactor()->remove_handler(this->event_->handle(),
                            ACE_Event_Handler::DONT_CALL);

  reactor()->cancel_timer(this);

  return 0;
}

#endif /* ACE_WIN32 && !ACE_LACKS_WIN32_SERVICES */
