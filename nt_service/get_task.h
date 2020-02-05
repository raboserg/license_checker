#pragma once
//#include "ace/Mutex.h"
#include "ace/Reactor.h"
#include "ace/Task.h"

class Get_Task_T : public ACE_Task<ACE_MT_SYNCH> {
public:
  Get_Task_T(ACE_Thread_Manager *thr_mgr, int n_threads);

  virtual int svc(void);
  //virtual int handle_signal(int, siginfo_t *siginfo, ucontext_t *);
  virtual int handle_timeout(const ACE_Time_Value &tv, const void *arg);
	virtual int handle_exception(ACE_HANDLE h);

private:
  void shutdown();
	int stop_;
  int n_threads_;
  ACE_SYNCH_MUTEX lock_;
  ACE_Array<ACE_CString> results_;
  ACE_SYNCH_CONDITION signal_;
};