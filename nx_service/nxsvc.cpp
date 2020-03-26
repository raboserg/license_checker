#include "nxsvc.h"
#include "ace/Date_Time.h"
#include "config_change.h"
#include "itvpn_exec_handler.h"
#include "parser_ini.h"
#include "tracer.h"

#include "ace/Get_Opt.h"
#include "ace/OS_NS_string.h"

namespace itvpnagent {

using namespace std;

ACE_TCHAR config_file[MAXPATHLEN];

static ACE_THR_FUNC_RETURN event_loop (void *arg) {
  ACE_Reactor *reactor = static_cast<ACE_Reactor *> (arg);
  reactor->owner (ACE_OS::thr_self ());
  reactor->run_reactor_event_loop ();
  return 0;
}

static int parse_args(int argc, ACE_TCHAR *argv[]) {
  static const ACE_TCHAR options[] = ACE_TEXT(":f:");
  ACE_Get_Opt cmd_opts(argc, argv, options, 1, 0, ACE_Get_Opt::PERMUTE_ARGS, 1);
  if (cmd_opts.long_option(ACE_TEXT("config"), 'f',
                           ACE_Get_Opt::ARG_REQUIRED) == -1)
    return -1;
  int option;
  ACE_OS_String::strcpy(config_file, ACE_TEXT("HAStatus.conf"));
  while ((option = cmd_opts()) != EOF) switch (option) {
      case 'f':
        ACE_OS_String::strncpy(config_file, cmd_opts.opt_arg(), MAXPATHLEN);
        break;
      case ':':
        ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("-%c requires an argument\n"),
                          cmd_opts.opt_opt()),
                         -1);
      default:
        ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Parse error.\n")), -1);
    }
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("Config file is %s\n"), config_file));
  return 0;
}

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

int Service::reshedule_tasks() {
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T Service: reshedule_tasks (%t) \n")));
  const Options options = PARSER::instance()->options();
  get_license_task_->set_day_waiting_hours(options.next_day_waiting_hours);
  get_license_task_->set_try_get_license_mins(
      options.next_try_get_license_mins);
  get_license_task_->schedule_handle_timeout(5);

  process_killer_task_->process_stopping_name(options.kill_file_name);
  process_killer_task_->set_day_waiting_hours(options.next_day_waiting_hours);
  process_killer_task_->schedule_handle_timeout(5);

  // reactor()->schedule_timer(get_license_task_, 0, tv1, ACE_Time_Value::zero);
  return 0;
}

int Service::run(int argc, char *argv[]) {
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T Start Service::svc (%t) \n")));
  DEBUG_LOG(TM("Start Service::svc"));

  if (parse_args(argc, argv) == -1 ||
      PARSER::instance()->init(utility::string_t(config_file)) == -1) {
    raise(SIGINT);
  }

  reactor()->owner(ACE_Thread::self());

  // Handle signals through the ACE_Reactor.
  if (this->reactor()->register_handler(SIGINT, &this->done_handler_) == -1) {
    ACE_ERROR(
        (LM_ERROR, "%T %p:\tcannot to register_handler SIGINT\t (%t) \n"));
    reactor()->notify(this, ACE_Event_Handler::EXCEPT_MASK);
  }

  const Options options = PARSER::instance()->options();

  //  const std::shared_ptr<LinuxNoficitator> notificator_ =
  //      std::make_shared<LinuxNoficitator>();
  //  const char *notify_paths[] = {options.openvpn_file_path.c_str(),
  //                                options.service_path.c_str()};
  //  if (notificator_->run_notify(2, notify_paths)) {
  //    ACE_ERROR(
  //        (LM_ERROR,
  //         "%T (%t) %p:\tcannot to initialize notificator for event sink\n"));
  //    reactor()->notify(this, ACE_Event_Handler::EXCEPT_MASK);
  //  }

  this->get_license_task_ =
      std::make_unique<Get_License_Task>(options.next_try_get_license_mins, 
      options.next_day_waiting_hours);
  if (this->get_license_task_->open(ACE_Time_Value(5)) == -1) {
    ACE_ERROR((LM_ERROR, "%T %p:\tcannot to open get_license_task\t (%t)\n"));
    reactor()->notify(this, ACE_Event_Handler::EXCEPT_MASK);
  }

  this->process_killer_task_ = std::make_unique<Process_Killer_Task>();
  this->process_killer_task_->process_stopping_name(options.kill_file_name);
  this->process_killer_task_->set_day_waiting_hours(options.next_day_waiting_hours);
  if (this->process_killer_task_->open(ACE_Time_Value(5, 0)) == -1) {
    ACE_ERROR(
        (LM_ERROR, "%T %p:\tcannot to open process_killer_task\t (%t) \n"));
    reactor()->notify(this, ACE_Event_Handler::EXCEPT_MASK);
  }
  
  ACE_Reactor notify_reactor;
  ACE_Thread_Manager::instance ()->spawn (event_loop, &notify_reactor);  
  
  const std::unique_ptr<Config_Handler> config_handler_ =
      std::make_unique<Config_Handler>(&notify_reactor);

  const std::unique_ptr<Itvpn_Exec_Handler> itvpn_exec_handler_ =
      std::make_unique<Itvpn_Exec_Handler>(&notify_reactor);
  
  reactor()->run_event_loop();
  notify_reactor.end_reactor_event_loop();
  
  ACE_Thread_Manager::instance()->wait(new ACE_Time_Value(3));
  // this->msg_queue();
  // Cleanly terminate connections, terminate threads.
  ACE_DEBUG((LM_SHUTDOWN, ACE_TEXT("%T Shutting down service (%t) \n")));
  INFO_LOG(TM("Shutting down service"));

  // notificator_->Release();
  return 0;
}

int Service::svc(void) { return 0; }

}  // namespace itvpnagent
