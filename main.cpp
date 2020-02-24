#pragma once
#include <iostream>

#ifdef _WIN32
#include "initializer.h"
#else
#include "notificator_linux.h"
#endif

#include <constants.h>
#include <tracer.h>

#include "ace/Date_Time.h"
#include "ace/Event_Handler.h"
#include "ace/Log_Msg.h"
#include "ace/Reactor.h"
#include "ace/Synch.h" // needed for ACE_Event

// ACE_Event timer_;
ACE_Event *timer_ = new ACE_Event;

class TestEvent : public ACE_Event_Handler {
public:
  TestEvent() {}

  int handle_signal(int signum, siginfo_t * = 0, ucontext_t * = 0) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("handle_timeout")));
    ACE_Reactor::instance()->end_event_loop();
    return 0;
  }
};

class TestTimer : public ACE_Event_Handler {
public:
  int handle_timeout(const ACE_Time_Value &, const void *arg) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("handle_timeout")));
    timer_->signal();
    return 0;
  }
};

int main(int argc, char *argv[]) {

  //  ACE_Event timer_;
  TestTimer testTimer;
  ACE_Reactor::instance()->schedule_timer(&testTimer, 0, ACE_Time_Value(3, 0),
                                          ACE_Time_Value::zero);
  TestEvent testEvent;
  // ACE_Reactor::instance()->register_handler(&testEvent, timer_.handle());
  ACE_Reactor::instance()->register_handler(&testEvent, timer_->handle());

  ACE_Reactor::instance()->run_event_loop();
  // timer_.wait();
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("After event...")));

#ifdef _WIN32
  return PROCESS::instance()->run(argc, argv);
#else
  LinuxNoficitator linuxNoficitator_;
  linuxNoficitator_.run_notify(argc, argv);
#endif

  // setlocale(LC_ALL, "ru_RU.UTF-8");
  return 0;
}
