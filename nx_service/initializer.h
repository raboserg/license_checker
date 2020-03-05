#pragma once
#include "nxsvc.h"
#include <ace/Get_Opt.h>
#include <ace/Init_ACE.h>

#include "ace/OS_NS_errno.h"
#include "ace/streams.h"

class Process {
public:
  Process(void);
  ~Process(void);

  int run(int argc, char *argv[]);

private:
  void parse_args(int argc, char *argv[]);
  void print_usage_and_die(void);

private:
  char progname[128];

  int opt_install;
  int opt_remove;
  int opt_start;
  int opt_kill;
  int opt_type;
  int opt_debug;

  int opt_startup;
};

typedef ACE_Singleton<Process, ACE_Mutex> PROCESS;
