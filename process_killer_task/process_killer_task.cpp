#include "process_killer_task.h"
#include "ace/OS_NS_time.h"
#include "ace/Process.h"
#include "constants.h"
#include "message_sender.h"
#include "tools.h"
#include "tracer.h"

#include "ace/OS_NS_signal.h"
#include "ace/Signal.h"

namespace itvpnagent {

Process_Killer_Task::Process_Killer_Task()
    : ACE_Task<ACE_MT_SYNCH>(ACE_Thread_Manager::instance()),
      licenseChecker_(new LicenseChecker()) {
  this->reactor(ACE_Reactor::instance());
}

Process_Killer_Task::Process_Killer_Task(ACE_Thread_Manager *thr_mgr)
    : ACE_Task<ACE_MT_SYNCH>(thr_mgr) {
  this->reactor(ACE_Reactor::instance());
}

Process_Killer_Task::~Process_Killer_Task() {
  reactor()->cancel_timer(this);
  ACE_DEBUG((LM_INFO, ACE_TEXT("%T ~Process_Killer_Task() (%t)\n")));
}

int Process_Killer_Task::open(ACE_Time_Value tv1) {
  // interval - ???
  this->timerId_ =
      reactor()->schedule_timer(this, 0, tv1, ACE_Time_Value::zero);
  return 0;
}

int Process_Killer_Task::close(const u_long arg) {
  ACE_DEBUG((LM_INFO,
             ACE_TEXT("%T Process_Killer_Task: close, arg - %d :(%t) \n"),
             arg));
  return 0;
}

int Process_Killer_Task::handle_timeout(const ACE_Time_Value &current_time,
                                        const void *) {
  time_t epoch = ((timespec_t)current_time).tv_sec;
  ACE_DEBUG((LM_INFO,
             ACE_TEXT("%T Process_Killer_Task: handle timeout :(%t) %s"),
             ACE_OS::ctime(&epoch)));
  if (this->activate(THR_NEW_LWP) == -1)
    ACE_ERROR_RETURN(
        (LM_ERROR,
         ACE_TEXT("%T Process_Killer_Task: activate failed :(%t) \n")),
        -1);
  return 0;
}

int Process_Killer_Task::handle_exception(ACE_HANDLE) {
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("%T Process_Killer_Task::handle_exception() (%t) \n")));
  return -1;
}

int Process_Killer_Task::svc() {
  ACE_DEBUG(
      (LM_INFO, ACE_TEXT("%T Process_Killer_Task: task started :(%t) \n")));
  try {
    if (licenseChecker_->is_license_check_day() &&
        (!licenseChecker_->is_license_file() ||
         !licenseChecker_->verify_license())) {
      if (System::terminate_process(this->process_stopping_name())) {
        const string_t message =
            this->process_stopping_name() + _XPLATSTR(" was killed");
        INFO_LOG(message.c_str());
        const string_t messa = _XPLATSTR("{ \"code\": \"1\", \"disc\": \"") +
                               message + _XPLATSTR("\" }");
        Net::send_message(messa);
        //MESSAGE_SENDER::instance()->send(messa);
      } else {
        const string_t message = _XPLATSTR("Process ") +
                                 this->process_stopping_name() +
                                 _XPLATSTR(" not found");
        const string_t messa = _XPLATSTR("{ \"code\": \"1\", \"disc\": \"") +
                               message + _XPLATSTR("\" }");
        INFO_LOG(message.c_str());
        Net::send_message(messa);
        //MESSAGE_SENDER::instance()->send(messa);
      }
    }
    schedule_handle_timeout(next_day_waiting_secs());
  } catch (const boost::process::process_error &err) {
    std::string str(err.what());
    const string_t message =
        conversions::to_string_t(str.substr(0, str.find_first_of(":")));
    //MESSAGE_SENDER::instance()->send(_XPLATSTR("0#Critical#") + message);
    Net::send_message(_XPLATSTR("0#Critical#") + message);
    ACE_ERROR((LM_DEBUG, ACE_TEXT("%T Process_Killer_Task: %s :(%t) \n"),
               err.what()));
    ERROR_LOG(message.c_str());
    // shutdown service
    raise(SIGINT);
  } catch (const std::runtime_error &err) {
    const string_t message = conversions::to_string_t(std::string(err.what()));
    //MESSAGE_SENDER::instance()->send(_XPLATSTR("0#Critical#") + message);
    Net::send_message(_XPLATSTR("0#Critical#") + message);
    ACE_ERROR((LM_DEBUG, ACE_TEXT("%T Process_Killer_Task: kill task :(%t) \n"),
               err.what()));
    ERROR_LOG(message.c_str());
    // shutdown service
    raise(SIGINT);
  }
  ACE_DEBUG(
      (LM_INFO, ACE_TEXT("%T Process_Killer_Task: task finished (%t) \n")));
  return 0;
}

int Process_Killer_Task::execute_process(
    const utility::string_t &process_name) {
  ACE_Process_Options options;
  // options.enable_unicode_environment();
  // options.setenv(ACE_TEXT("ZZ"), ACE_TEXT("1"));
  options.command_line(process_name.c_str());
  ACE_Process process;
  if (process.spawn(options) == -1)
    ACE_ERROR_RETURN(
        (LM_ERROR, ACE_TEXT("%T Process_Killer_Task: "
                            "ERROR: Failed to spawn process :(%t) \n")),
        -1);
  ACE_exitcode status;
  process.wait(&status);
  if (status != 1)
    ACE_ERROR_RETURN(
        (LM_ERROR, ACE_TEXT("%T Process_Killer_Task: "
                            "ERROR: Failed status of spawn process :(%t) \n")),
        -1);
  return 0;
}

int Process_Killer_Task::schedule_handle_timeout(const int &seconds) {
  ACE_Time_Value tv1(seconds, 0);
  // reactor()->reset_timer_interval(this->timerId_, tv1);
  reactor()->cancel_timer(this);
  timerId_ = reactor()->schedule_timer(this, 0, tv1, ACE_Time_Value::zero);
  return 0;
}

int Process_Killer_Task::shutdown_service() {
  reactor()->cancel_timer(this);
  return reactor()->end_event_loop();
}

} // namespace itvpnagent