#pragma once

#include "ace/Reactor.h"
#include "ace/Task.h"

#include "ace/Timer_Heap.h"
#include "license_checker.h"

class Get_Update_Task : public ACE_Task<ACE_MT_SYNCH> {
public:
  Get_Update_Task();
  Get_Update_Task(ACE_Thread_Manager *thr_mgr, const int n_threads);
  virtual ~Get_Update_Task();
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
  ACE_Array<ACE_CString> results_;
  const std::unique_ptr<LicenseChecker> licenseChecker_;

  int shutdown_service() {
    reactor()->cancel_timer(this);
    return reactor()->end_reactor_event_loop();
  }
};
