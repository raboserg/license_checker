#pragma once

#include "ace/config-lite.h"

#if defined(ACE_WIN32) && !defined(ACE_LACKS_WIN32_SERVICES)

#include "ace/Event_Handler.h"
#include "ace/Mutex.h"
#include "ace/NT_Service.h"
#include "ace/Reactor.h"
#include "ace/Sig_Adapter.h"
#include "ace/Singleton.h"
#include "ace/WFMO_Reactor.h"
#include "event_sink_task.h"
#include "notificator.h"

#include "get_license_task.h"
#include "process_killer_task.h"

class Service : public ACE_NT_Service {
public:
  Service(void);

  ~Service(void);

  //???int open(void *args);

  virtual int handle_close(ACE_HANDLE, ACE_Reactor_Mask);

  /// We override <handle_control> because it handles stop requests
  /// privately.
  virtual void handle_control(DWORD control_code);

  /// Handle events being signaled by the main thread.
  int handle_signal(int signum, siginfo_t * = 0, ucontext_t * = 0);

  /// We override <handle_exception> so a 'stop' control code can pop
  /// the reactor off of its wait.
  virtual int handle_exception(ACE_HANDLE h);

  /// This is a virtual method inherited from ACE_NT_Service.
  virtual int svc(void);

  /// Where the real work is done:
  virtual int handle_timeout(const ACE_Time_Value &tv, const void *arg = 0);

private:
  typedef ACE_NT_Service inherited;

  int get_seconds_in_hours(const int days) {
    const int sec_min = 60;
    const int min_hour = 60;
    const int hour_day = 24;
    return sec_min * min_hour * hour_day * days;
  }

private:
  int stop_;
  ACE_Sig_Adapter done_handler_;
  std::shared_ptr<ACE_Auto_Event> event_;
  std::shared_ptr<WinNT::Notificator> notificator_;
  std::unique_ptr<Get_License_Task> get_license_task_;
  std::unique_ptr<Process_Killer_Task> process_killer_task_;
};

// Define a singleton class as a way to insure that there's only one
// Service instance in the program, and to protect against access from
// multiple threads.  The first reference to it at runtime creates it,
// and the ACE_Object_Manager deletes it at run-down.

typedef ACE_Singleton<Service, ACE_Mutex> SERVICE;

#endif /* ACE_WIN32 && !ACE_LACKS_WIN32_SERVICES */
