#pragma once
#include "ace/Singleton.h"
#include "ace/Synch.h"
#include "ace/Task.h"


class EventSink_Task : public ACE_Task<ACE_SYNCH> {
public:
  enum { MAX_THREADS = 1 };

	EventSink_Task();
  virtual int open(void * = 0) { return activate(THR_NEW_LWP, MAX_THREADS); }

  virtual int put(ACE_Message_Block *mblk, ACE_Time_Value *timeout = 0) {
    return putq(mblk, timeout);
  }
	
	virtual int svc();
};

typedef ACE_Unmanaged_Singleton<EventSink_Task, ACE_Null_Mutex>
    LICENSE_WORKER_TASK;
