#pragma once

#include "ace/Reactor.h"
#include "ace/Task.h"

#include "ace/Timer_Heap.h"
#include "ace/Timer_Queue_Adapters.h"
#include "license_checker.h"

class Process_Killer_Task : public ACE_Task<ACE_MT_SYNCH> {
public:
  Process_Killer_Task();
  Process_Killer_Task(ACE_Thread_Manager *thr_mgr);
  virtual ~Process_Killer_Task();
  int close(u_long arg);
  int open(ACE_Time_Value tv1);
  // virtual int handle_signal(int, siginfo_t *siginfo, ucontext_t *);
  virtual int handle_exception(ACE_HANDLE h);
  virtual int handle_timeout(const ACE_Time_Value &tv, const void *arg);
  virtual int schedule_handle_timeout(const int &seconds);

  string_t process_stopping_name() { return process_stopping_name_; }

  void process_stopping_name(const string_t process_name) {
    process_stopping_name_ = process_name;
  }

private:
  long timerId_;
  string_t process_stopping_name_;

  ACE_Array<ACE_CString> state_;
  const std::unique_ptr<LicenseChecker> licenseChecker_;
  virtual int svc(void);
  int shutdown_service();
  int terminate_process(const utility::string_t filename);
  int execute_process(const utility::string_t &filename);
};
