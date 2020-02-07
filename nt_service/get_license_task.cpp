#include "get_license_task.h"
#include "client_license.h"
#include "license_checker.h"

Get_License_Task::Get_License_Task()
    : ACE_Task<ACE_MT_SYNCH>(ACE_Thread_Manager::instance()), condition_(lock_),
      n_threads_(1) {
	this->reactor(ACE_Reactor::instance());
}

Get_License_Task::Get_License_Task(ACE_Thread_Manager *thr_mgr,
                                   const int n_threads)
    : ACE_Task<ACE_MT_SYNCH>(thr_mgr), n_threads_(n_threads),
      condition_(lock_) {
  reactor(ACE_Reactor::instance());
}

Get_License_Task::~Get_License_Task() {
  close();
  ACE_DEBUG((LM_INFO, ACE_TEXT("%T (%t):\t~Get_License_Task()\n")));
}

int Get_License_Task::open(ACE_Time_Value tv1) {
  this->done_ = false;
  reactor()->schedule_timer(this, 0, tv1, tv1);
  if (this->activate(THR_NEW_LWP, n_threads_) == -1)
    ACE_ERROR_RETURN(
        (LM_ERROR, ACE_TEXT("%T (%t):\tGet_License_Task: activate failed")),
        -1);
  else
    ACE_DEBUG((LM_INFO,
               ACE_TEXT("%T (%t):\tGet_License_Task: started %d threads\n"),
               n_threads_));
  return 0;
}

void Get_License_Task::close() {
  this->done_ = true;
  reactor()->cancel_timer(this);
  ACE_DEBUG((LM_INFO, ACE_TEXT("%T (%t):\tGet_License_Task: cancel timer\n")));
}

int Get_License_Task::handle_timeout(const ACE_Time_Value &tv, const void *) {
  ACE_UNUSED_ARG(tv);
  this->condition_.signal();
  ACE_DEBUG(
      (LM_DEBUG, ACE_TEXT("%T (%t):\tGet_License_Task::handle_timeout\n")));
  return 0;
}

bool Get_License_Task::done(void) const { return done_; }

int Get_License_Task::handle_exception(ACE_HANDLE) {
  ACE_DEBUG(
      (LM_DEBUG, ACE_TEXT("%T (%t):\tGet_License_Task::handle_exception()\n")));
  this->done_ = false;
  return -1;
}

int Get_License_Task::svc() {
	ACE_DEBUG((LM_INFO, ACE_TEXT("%T (%t):\tGet_License_Task: task started\n")));

	while (!done()) {
    condition_.wait();
    
		const std::unique_ptr<LicenseChecker> licenseChecker_ =
        std::make_unique<LicenseChecker>();

    ACE_DEBUG(
        (LM_INFO, ACE_TEXT("%T (%t):\tGet_License_Task: task running\n")));

    try {
      if (licenseChecker_->check_day_updete())
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("%T (%t):\tGet_License_Task: update license\n")));
    } catch (const std::runtime_error &err) {
      ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T (%t):\tShutting down service\n")));
      reactor()->end_reactor_event_loop();
      // CRITICAL_LOG(TM("KEY OF LICENSE_DAY_FOR_UPDATE is failed"));
    }
  }
  ACE_DEBUG((LM_INFO, ACE_TEXT("%T (%t):\tGet_License_Task: task finished\n")));
  return 0;
}
