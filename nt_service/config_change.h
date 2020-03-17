#pragma once

#include "ace/Auto_Event.h"
#include "ace/Log_Msg.h"
#include "ace/OS_NS_unistd.h"
#include "ace/Reactor.h"
#include "ace/Task.h"
#include "ace/Thread_Mutex.h"
#include <Windows.h>

namespace itvpnagent {

class Config_Handler : public ACE_Task_Base {
public:
  Config_Handler(ACE_Reactor *reactor);
  ~Config_Handler(void);

  virtual int handle_signal(int signum, siginfo_t * = 0, ucontext_t * = 0);
  virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);
  virtual int handle_timeout(const ACE_Time_Value &tv, const void *arg);

  ACE_WString get_directory();
  void set_directory(ACE_WString &directory);

  ACE_WString get_file_name();
  void set_file_name(ACE_WString &file_name);

  virtual int open();
  // virtual int close(u_long flags = 0);

private:
  bool bStop;
  int count_;
  ACE_HANDLE handle_;
  ACE_WString file_name_;
  ACE_WString directory_;
  ACE_Auto_Event *event_;
  ACE_Auto_Event *active_handler_;
  ACE_HANDLE get_handle(void) const;

  int create_file();
  virtual int svc(void);
  int processing(const BYTE *lpBuffer, const DWORD nBufferLength);
};

} // namespace itvpnagent