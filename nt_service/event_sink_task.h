#pragma once
#include "ace/Singleton.h"
#include "ace/Synch.h"
#include "ace/Task.h"
#include "license_checker.h"

class EventSink_Task : public ACE_Task<ACE_SYNCH> {
	const unique_ptr<LicenseChecker> licenseChecker_ ;
	int write_license(const shared_ptr<HostLicense> &host_license);

public:
  EventSink_Task();
  virtual int open(void * = 0) { return activate(THR_NEW_LWP); }

  virtual int put(ACE_Message_Block *mblk, ACE_Time_Value *timeout = 0) {
    return putq(mblk, timeout);
  }

  virtual int svc();
};

typedef ACE_Unmanaged_Singleton<EventSink_Task, ACE_Null_Mutex>
    LICENSE_WORKER_TASK;
