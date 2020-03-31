#pragma once

#include "ace/Log_Msg.h"
#include "ace/OS_NS_unistd.h"
#include "ace/Reactor.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/inotify.h>
#include <sys/types.h>

namespace itvpnagent {

using namespace std;

#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

class Itvpn_Exec_Handler : public ACE_Event_Handler {
public:
  Itvpn_Exec_Handler(ACE_Reactor *reactor);
  ~Itvpn_Exec_Handler(void);
  int handle_input(ACE_HANDLE fd);
  int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);
  ACE_HANDLE get_handle(void) const;

  string get_directory() const;
  void set_directory(const string &directory);

  string get_file_name() const;
  void set_file_name(const string &file_name);

private:
  int watch_;
  int iterations_;
  string file_name_;
  string directory_;
  ACE_HANDLE handle_;
};

} // namespace itvpnagent
