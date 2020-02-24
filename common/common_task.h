#pragma once

typedef ACE_Task<ACE_MT_SYNCH> MT_Task;

class Common_Task : public MT_Task
{
public:
  Common_Task (void) {}
  virtual int open (void * = 0);
  virtual int close (u_long = 0);
	virtual int shutdown();
};
