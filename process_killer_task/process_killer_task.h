#pragma once

#include "ace/Reactor.h"
#include "ace/Task.h"

#include "ace/Timer_Heap.h"
#include "ace/Timer_Queue_Adapters.h"
#include "license_checker.h"

class Process_Killer_Task : public ACE_Task<ACE_MT_SYNCH> {
public:
	Process_Killer_Task();
	Process_Killer_Task(ACE_Thread_Manager *thr_mgr, const int n_threads);
  virtual ~Process_Killer_Task();
  void close();
  virtual int svc(void);
  int open(ACE_Time_Value tv1);
  // virtual int handle_signal(int, siginfo_t *siginfo, ucontext_t *);
  virtual int handle_exception(ACE_HANDLE h);
  virtual int handle_timeout(const ACE_Time_Value &tv, const void *arg);
  virtual int schedule_handle_timeout(const int &seconds);

private:
	long timerId_;
	int n_threads_;
  ACE_Array<ACE_CString> state_;
  const std::unique_ptr<LicenseChecker> licenseChecker_;
	int terminate_process(utility::string_t filename);
	int shutdown_service();
};
