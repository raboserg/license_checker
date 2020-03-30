#pragma once

#include "ace/Reactor.h"
#include "ace/Task.h"
#include "ace/Timer_Heap.h"
#include "ace/Timer_Queue_Adapters.h"

#include "license_checker.h"
//#include "common_task.h"
//#include "event_sink_task.h"
//#include "ace/Reactor_Notification_Strategy.h"

using Thread_Timer_Queue = ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap>;

struct MyActiveTimer : public Thread_Timer_Queue {
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

namespace itvpnagent {

class Get_License_Task : public ACE_Task<ACE_MT_SYNCH> {
public:
  Get_License_Task(const int &try_get_license_mins, const int &waiting_hours);
  Get_License_Task(ACE_Thread_Manager *thr_mgr, const int &try_get_license_mins,
                   const int &waiting_hours);
  virtual ~Get_License_Task();
  int close(u_long arg);
  int open(const ACE_Time_Value tv1);
  // virtual int handle_signal(int, siginfo_t *siginfo, ucontext_t *);
  virtual int handle_exception(ACE_HANDLE h);
  virtual int handle_timeout(const ACE_Time_Value &tv, const void *arg);
  virtual int schedule_handle_timeout(const int &seconds);

  void set_try_get_license_mins(const int license_mins) {
    try_get_license_mins_ = license_mins;
  }

  void set_day_waiting_hours(const int waiting_hours) {
    day_waiting_hours_ = waiting_hours;
  }

private:
  long timerId_;
  int day_counter_;
  int try_get_license_mins_;
  int day_waiting_hours_;
  ACE_Array<ACE_CString> results_;
  const std::unique_ptr<LicenseChecker> licenseChecker_;

  virtual int svc(void);
  int next_try_get_license_secs() { return try_get_license_mins_ * 60; }
  int next_day_waiting_secs() { return day_waiting_hours_ * 60 * 60; }
  int try_get_license_mins() { return try_get_license_mins_; }
  int day_waiting_hours() { return day_waiting_hours_; }
  void inform_next_try_log() {
    // char_t log[100];
    std::array<char_t, 100> log;
    const size_t fmt_len = ACE_OS::sprintf(
        log.data(),
        _XPLATSTR("The next attempt to get a license in %d minutes"),
        this->try_get_license_mins_);
    INFO_LOG(log.data());
  }

  virtual int write_license(const shared_ptr<HostLicense> &host_license);
};
} // namespace itvpnagent
