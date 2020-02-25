#pragma once

typedef ACE_Task<ACE_MT_SYNCH> MT_Task;

class Common_Task : public MT_Task {
public:
  Common_Task(void);
  virtual int open();
  virtual int close();
  virtual int shutdown();
};

Common_Task::Common_Task(void) {}

int Common_Task::open() { return 0; }

int Common_Task::close() { return 0; }

int Common_Task::shutdown() { return 0; }
