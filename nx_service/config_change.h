#pragma once

#include "ace/Log_Msg.h"
#include "ace/OS_NS_unistd.h"
#include "ace/Reactor.h"
#include "ace/Sig_Adapter.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <sys/types.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))
static const ACE_TCHAR *file = ACE_TEXT("ittas.ini");
static const ACE_TCHAR *directory = ACE_TEXT("/home/user/pt");

class Config_Handler : public ACE_Event_Handler {
public:
  Config_Handler(ACE_Reactor *reactor);
  ~Config_Handler(void);
  int handle_signal(int signum, siginfo_t * = 0, ucontext_t * = 0);
  int handle_input(ACE_HANDLE fd);
  int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);
  ACE_HANDLE get_handle(void) const;

private:
  int wd_;
  ACE_HANDLE handle_;
  // ACE_Sig_Adapter done_handler_;
};
