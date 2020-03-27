#pragma once

#include "ace/Auto_Event.h"
#include "ace/Mutex.h"
#include "ace/Sig_Adapter.h"
#include "ace/Singleton.h"
#include "ace/Task.h"
#include "ace/config-lite.h"
#include "get_license_task.h"
#include "process_killer_task.h"

namespace itvpnagent {
using namespace std;

class Service : public ACE_Task<ACE_MT_SYNCH> {
public:
  Service(void);
  ~Service(void);
  virtual int run(int argc, char *argv[]);
  virtual int svc(void);
  int reshedule_tasks();
  virtual int handle_exception(ACE_HANDLE h);
  virtual int handle_close(ACE_HANDLE, ACE_Reactor_Mask);
  virtual int handle_timeout(const ACE_Time_Value &tv, const void *arg = 0);

private:
  int stop_;
  ACE_Sig_Adapter done_handler_;

  shared_ptr<ACE_Event> event_;
  unique_ptr<Get_License_Task> get_license_task_;
  unique_ptr<Process_Killer_Task> process_killer_task_;
};

} // namespace itvpnagent
typedef ACE_Singleton<itvpnagent::Service, ACE_Mutex> SERVICE;
