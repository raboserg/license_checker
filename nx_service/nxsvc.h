#pragma once

#include "ace/Mutex.h"
#include "ace/Sig_Adapter.h"
#include "ace/Singleton.h"
#include "ace/Task.h"
//???#include "ace/Synch.h"
#include "ace/config-lite.h"
#include "notificator_linux.h"

#include "get_license_task.h"
#include "process_killer_task.h"

class Service : public ACE_Task<ACE_MT_SYNCH> {
public:
  Service(void);
  ~Service(void);
  virtual int run();
  virtual int handle_close(ACE_HANDLE, ACE_Reactor_Mask);
  virtual int handle_exception(ACE_HANDLE h);
  virtual int svc(void);
  virtual int handle_timeout(const ACE_Time_Value &tv, const void *arg = 0);

private:
  int stop_;
  ACE_Sig_Adapter done_handler_;
  //???std::shared_ptr<ACE_Event> event_;
  // std::shared_ptr<WinNT::Notificator> notificator_;
  // std::unique_ptr<Get_License_Task> get_license_task_;
  // std::unique_ptr<Process_Killer_Task> process_killer_task_;
};

typedef ACE_Singleton<Service, ACE_Mutex> SERVICE;
