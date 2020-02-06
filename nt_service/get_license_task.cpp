#include "get_license_task.h"

Get_License_Task::Get_License_Task()
    : ACE_Task<ACE_MT_SYNCH>(ACE_Thread_Manager::instance()), condition_(lock_),
      n_threads_(1) {
	open();
}

Get_License_Task::Get_License_Task(ACE_Thread_Manager *thr_mgr, const int n_threads)
    : ACE_Task<ACE_MT_SYNCH>(thr_mgr), n_threads_(n_threads), condition_(lock_) {
	open();
}

Get_License_Task::~Get_License_Task() {
	//???shutdown(); //for end_event_loop()
}

void Get_License_Task::open() {
	this->stop_ = 0;
	if (this->activate(THR_NEW_LWP, n_threads_) == -1)
		ACE_ERROR((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("activate failed")));
	else
		ACE_DEBUG((LM_INFO, ACE_TEXT("(%P|%t) started %d threads\n"), n_threads_));
}

void Get_License_Task::shutdown() {
  ACE_GUARD(ACE_SYNCH_MUTEX, guard_, lock_);
  --n_threads_;
  if (n_threads_ <= 0) {
    ACE_DEBUG((LM_INFO, ACE_TEXT("(%P|%t) ending event loop\n")));
    ACE_Reactor::instance()->end_event_loop();
  }
}
int Get_License_Task::handle_timeout(const ACE_Time_Value &tv, const void *) {
  ACE_UNUSED_ARG(tv);

  this->condition_.signal();

  ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T (%t): Get_License_Task::handle_timeout\n")));
  return 0;
}

bool Get_License_Task::done(void) const {
	return stop_;
}

int Get_License_Task::handle_exception(ACE_HANDLE) {
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("Get_License_Task::handle_exception(ACE_HANDLE)\n")));
  this->stop_ = false;
  return -1;
}

int Get_License_Task::svc() {
  ACE_DEBUG((LM_INFO, ACE_TEXT("(%P|%t) task started\n")));
  while (!done()) {
		condition_.wait();

    ACE_DEBUG((LM_INFO, ACE_TEXT("(%P|%t) task running\n")));
  }
  ACE_DEBUG((LM_INFO, ACE_TEXT("(%P|%t) task finished\n")));

  return 0;
}
