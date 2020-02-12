#pragma once

#include "ace/Reactor.h"
#include "ace/Task.h"

//#include "ace/Reactor_Notification_Strategy.h"
#include "ace/Timer_Heap.h"
#include "ace/Timer_Queue_Adapters.h"
#include "license_checker.h"
//#include "event_sink_task.h"

class Get_License_Task;

class MyActiveTimer : public ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap> {
public:
  MyActiveTimer() { this->activate(); }
};

class CB : public ACE_Event_Handler {
public:
  CB(int id) : id_(id) {}

  virtual int handle_timeout(const ACE_Time_Value &, const void *arg) {
    ACE_TRACE("CB::handle_timeout");

    const int *val = static_cast<const int *>(arg);
    if ((*val) != id_)
      return -1;
    /////////////////////////////////////////////////
  /*  ACE_Message_Block *mblk = 0;
    ACE_Message_Block *log_blk = 0;

    ACE_NEW_RETURN(log_blk, ACE_Message_Block(reinterpret_cast<char *>(this)),
                   -1);
    log_blk->cont(mblk);

    LICENSE_WORKER_TASK::instance()->put(log_blk);*/
    /////////////////////////////////////////////////

    ACE_UNUSED_ARG(val);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("Expiry handled by thread %t %d\n"), id_));
    // Get_License_Task
    return 0;
  }

private:
  int id_;
};

class Get_License_Task : public ACE_Task<ACE_MT_SYNCH> {
public:
  Get_License_Task(ACE_Thread_Manager *thr_mgr, const int n_threads);
  virtual ~Get_License_Task();
  Get_License_Task();
  void close();
  virtual int svc(void);
  int open(ACE_Time_Value tv1);
  // virtual int handle_signal(int, siginfo_t *siginfo, ucontext_t *);
  virtual int handle_exception(ACE_HANDLE h);
  virtual int handle_timeout(const ACE_Time_Value &tv, const void *arg);
	virtual int schedule_wait(const int seconds);

private:
  int n_threads_;
  ACE_Array<ACE_CString> results_;
	const std::unique_ptr<LicenseChecker> licenseChecker_;
  // the Bridge/Strategy patterns.
  //???ACE_Reactor_Notification_Strategy notification_strategy_;
};
