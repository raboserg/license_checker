#include "get_task.h"

Get_Task_T::Get_Task_T(ACE_Thread_Manager *thr_mgr, int n_threads)
    : ACE_Task<ACE_MT_SYNCH>(thr_mgr), n_threads_(n_threads), signal_(lock_) {
  // Create worker threads.
  this->stop_ = 0;
  if (this->activate(THR_NEW_LWP, n_threads_) == -1)
    ACE_ERROR((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("activate failed")));
  else
    ACE_DEBUG((LM_INFO, ACE_TEXT("(%P|%t) started %d threads\n"), n_threads_));
}

void Get_Task_T::shutdown() {
  ACE_GUARD(ACE_SYNCH_MUTEX, guard_, lock_);
  --n_threads_;
  if (n_threads_ <= 0) {
    ACE_DEBUG((LM_INFO, ACE_TEXT("(%P|%t) ending event loop\n")));
    ACE_Reactor::instance()->end_event_loop();
  }
}
int Get_Task_T::handle_timeout(const ACE_Time_Value &tv, const void *) {
  ACE_UNUSED_ARG(tv);

  this->signal_.signal();

  ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T (%t): Get_Task_T::handle_timeout\n")));
  return 0;
}

int Get_Task_T::handle_exception(ACE_HANDLE) {
	ACE_DEBUG(
		(LM_DEBUG, ACE_TEXT("Get_Task_T::handle_exception(ACE_HANDLE)\n")));
	this->stop_ = 1;
	return -1;
}

int Get_Task_T::svc() {
  ACE_DEBUG((LM_INFO, ACE_TEXT("(%P|%t) task started\n")));

  while (!this->stop_) {
    signal_.wait();
    ACE_DEBUG((LM_INFO, ACE_TEXT("(%P|%t) task running\n")));
  }
  /*if (this->n) {
                                  }
                                  else
                                                                  ACE_ERROR((LM_ERROR,
  ACE_TEXT("(%P|%t) failed to open URL %C\n"),"")); shutdown(); }	else {
  ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) failed to parse URL : result =
  %C\n"),""));
  }*/

  ACE_DEBUG((LM_INFO, ACE_TEXT("(%P|%t) task finished\n")));

  return 0;
}
