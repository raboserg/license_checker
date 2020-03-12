#ifndef CONFIG_CHANGE_H
#define CONFIG_CHANGE_H

#include "ace/Auto_Event.h"
#include "ace/Log_Msg.h"
#include "ace/OS_NS_unistd.h"
#include "ace/Reactor.h"
#include "ace/Task.h"
#include <Windows.h>

static int stop_test = 0;
static const WCHAR *directory = L"D:\\work\\itvpn_setup\\itvpn\\bin\\x64\\";
static const WCHAR *temp_file = L"itvpnagent.ini";

class Config_Handler : public ACE_Task_Base {
public:
  Config_Handler(ACE_Reactor *reactor);
  ~Config_Handler(void);

  int handle_signal(int signum, siginfo_t * = 0, ucontext_t * = 0);
  int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);

  virtual int open();
  // virtual int close(u_long flags = 0);

private:
  bool bStop;
  ACE_HANDLE handle_;
  ACE_Auto_Event *event_;
  virtual int svc(void);
  ACE_HANDLE get_handle(void) const;
};

#endif /* ACE_WIN32 */
