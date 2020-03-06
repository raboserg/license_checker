#include "process_killer_task.h"
#include "ace/OS_NS_time.h"
#include "ace/Process.h"
#include "tracer.h"
#ifdef _WIN32
#include <Tlhelp32.h>
#include <process.h>
#endif // _WIN32
//#include "gason.h"
#include "constants.h"
#include "message_sender.h"

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

int Process_Killer_Task::close(u_long arg) { return 0; }

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
      if (terminate_process(this->process_stopping_name())) {
        const string_t message =
            this->process_stopping_name() + _XPLATSTR(" was killed");
        INFO_LOG(message.c_str());
        const string_t messa = _XPLATSTR("{ \"code\": \"1\", \"disc\": \"") +
                               message + _XPLATSTR("\" }");
        MESSAGE_SENDER::instance()->send(messa);
      } else {
        const string_t message = _XPLATSTR("Process ") +
                                 this->process_stopping_name() +
                                 _XPLATSTR(" not found");
        const string_t messa = _XPLATSTR("{ \"code\": \"1\", \"disc\": \"") +
                               message + _XPLATSTR("\" }");
        INFO_LOG(message.c_str());
        MESSAGE_SENDER::instance()->send(messa);
      }
    }
    // reschedule next day
    schedule_handle_timeout(lic::constants::NEXT_DAY_WAITING);
  } catch (const std::runtime_error &err) {
    CRITICAL_LOG(utility::conversions::to_string_t(err.what()).c_str());
    // shutdown_service(); //???

    raise(SIGINT); //???

    ACE_ERROR_RETURN(
        (LM_ERROR, ACE_TEXT("%T Process_Killer_Task: kill task - %s :(%t) \n"),
         err.what()),
        -1);
  }
  ACE_DEBUG(
      (LM_INFO, ACE_TEXT("%T Process_Killer_Task: task finished (%t) \n")));
  return 0;
}

int Process_Killer_Task::execute_process(const utility::string_t process_name) {
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

int Process_Killer_Task::terminate_process(const utility::string_t filename) {
  int hRes;
  bool founded = false;
#ifdef _WIN32
  WCHAR szPath[20];
  wcscpy_s(szPath, filename.c_str());

  HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
  PROCESSENTRY32 pEntry;
  pEntry.dwSize = sizeof(pEntry);
  hRes = Process32First(hSnapShot, &pEntry);
  while (hRes) {
    if (wcscmp(pEntry.szExeFile, szPath) == 0) {
      HANDLE hProcess =
          OpenProcess(PROCESS_TERMINATE, 0, (DWORD)pEntry.th32ProcessID);
      if (hProcess != NULL) {
        TerminateProcess(hProcess, 9);
        CloseHandle(hProcess);
        founded = true;
      }
    }
    hRes = Process32Next(hSnapShot, &pEntry);
  }
  CloseHandle(hSnapShot);
#endif
  return founded;
}
