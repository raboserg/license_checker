#include "nxsvc.h"
#include "ace/Date_Time.h"
#include "config_change.h"
#include "parser_ini.h"
#include "tracer.h"

Service::Service(void)
    : done_handler_(ACE_Sig_Handler_Ex(ACE_Reactor::end_event_loop)) {
  reactor(ACE_Reactor::instance());
}

Service::~Service(void) {}

int Service::handle_close(ACE_HANDLE, ACE_Reactor_Mask) {
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T \tService::handle_close \t (%t) \n")));
  reactor()->end_reactor_event_loop();
  return 0;
}

int Service::handle_exception(ACE_HANDLE) {
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T (%t):\tService::handle_exception()\n")));
  return -1;
}

int Service::handle_timeout(const ACE_Time_Value &tv, const void *) {
  ACE_UNUSED_ARG(tv);
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T (%t):\thandle timeout...\n")));
  return 0;
}

int Service::run(void) {
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T Start Service::svc (%t) \n")));
  DEBUG_LOG(TM("Start Service::svc"));

  reactor()->owner(ACE_Thread::self());

  // Handle signals through the ACE_Reactor.
  if (this->reactor()->register_handler(SIGINT, &this->done_handler_) == -1) {
    ACE_ERROR(
        (LM_ERROR, "%T %p:\tcannot to register_handler SIGINT\t (%t) \n"));
    reactor()->notify(this, ACE_Event_Handler::EXCEPT_MASK);
  }

  if (PARSER::instance()->init() == -1) {
    ACE_ERROR((LM_ERROR, "%T (%t) %p: cannot to initialize constants\n",
               "\tService::svc"));
    raise(SIGINT);
  }

  //  const std::shared_ptr<LinuxNoficitator> notificator_ =
  //      std::make_shared<LinuxNoficitator>();
  //  if (notificator_->srv_run( == -1) {
  //    ACE_ERROR(
  //        (LM_ERROR,
  //         "%T (%t) %p:\tcannot to initialize notificator for event sink\n"));
  //    reactor()->notify(this, ACE_Event_Handler::EXCEPT_MASK);
  //  }

  const std::unique_ptr<Config_Handler> config_handler_ =
      std::make_unique<Config_Handler>(ACE_Reactor::instance());

  const int try_get_license_mins = ACE_OS::atoi(
      PARSER::instance()->options().next_try_get_license_mins.c_str());
  const std::unique_ptr<Get_License_Task> get_license_task_ =
      std::make_unique<Get_License_Task>(try_get_license_mins);
  if (get_license_task_->open(ACE_Time_Value(5)) == -1) {
    ACE_ERROR((LM_ERROR, "%T %p:\tcannot to open get_license_task\t (%t)\n"));
    reactor()->notify(this, ACE_Event_Handler::EXCEPT_MASK);
  }

  const std::unique_ptr<Process_Killer_Task> process_killer_task_ =
      std::make_unique<Process_Killer_Task>();
  const string_t process_stopping_name =
      PARSER::instance()->options().kill_file_name;
  process_killer_task_->process_stopping_name(process_stopping_name);
  if (process_killer_task_->open(ACE_Time_Value(5, 0)) == -1) {
    ACE_ERROR(
        (LM_ERROR, "%T %p:\tcannot to open process_killer_task\t (%t) \n"));
    reactor()->notify(this, ACE_Event_Handler::EXCEPT_MASK);
  }
  this->reactor()->run_event_loop();
  // this->msg_queue();
  // Cleanly terminate connections, terminate threads.
  ACE_DEBUG((LM_SHUTDOWN, ACE_TEXT("%T Shutting down service (%t) \n")));
  INFO_LOG(TM("Shutting down service"));

  // notificator_->Release();
  return 0;
}

int Service::svc(void) { return 0; }
