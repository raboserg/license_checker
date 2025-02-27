#pragma once

#ifndef __CLIENT_H_
#define __CLIENT_H_

//#include "ace/Null_Condition.h"
//#include "ace/Null_Mutex.h"
//#include "ace/Synch_Traits.h"


#include "ace/Connector.h"
#include "ace/INET_Addr.h"
#include "ace/Reactor.h"
#include "ace/Reactor_Notification_Strategy.h"
#include "ace/SOCK_Connector.h"
#include "ace/SOCK_Stream.h"
#include "ace/Svc_Handler.h"

class Client : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> {
  typedef ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> super;

public:
  Client() : notifier_(0, this, ACE_Event_Handler::WRITE_MASK) {}

  virtual int open(void * = 0);

  // Called when input is available from the client.
  virtual int handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);

  // Called when output is possible.
  virtual int handle_output(ACE_HANDLE fd = ACE_INVALID_HANDLE);

  // Called when a timer expires.
  virtual int handle_timeout(const ACE_Time_Value &current_time,
                             const void *act = 0);

private:
  enum { ITERATIONS = 5 };
  int iterations_;
  ACE_Reactor_Notification_Strategy notifier_;
};

#endif /* __CLIENT_H_ */
