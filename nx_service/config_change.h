#pragma once

#include "ace/Log_Msg.h"
#include "ace/OS_NS_unistd.h"
#include "ace/Reactor.h"
#include "ace/Sig_Adapter.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/inotify.h>
#include <sys/types.h>

namespace itvpnagent {

#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

class Config_Handler : public ACE_Event_Handler {
public:
  Config_Handler(ACE_Reactor *reactor);
  ~Config_Handler(void);
  int handle_signal(int signum, siginfo_t * = 0, ucontext_t * = 0);
  int handle_input(ACE_HANDLE fd);
  int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);
  ACE_HANDLE get_handle(void) const;

  std::string get_directory();
  void set_directory(const std::string &directory);

  std::string get_file_name();
  void set_file_name(const std::string &file_name);

private:
  int watch_;
  ACE_HANDLE handle_;
  std::string file_name_;
  std::string directory_;
};

} // namespace itvpnagent
