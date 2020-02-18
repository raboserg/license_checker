#include "process_killer_task.h"
#include "ace/Process.h"
#include "tracer.h"
#ifdef _WIN32
#include <Tlhelp32.h>
#include <process.h>
#endif // _WIN32
#include "gason.h"
#include "parser_ini.h"

Process_Killer_Task::Process_Killer_Task()
    : ACE_Task<ACE_MT_SYNCH>(ACE_Thread_Manager::instance()), n_threads_(1),
      licenseChecker_(new LicenseChecker()) {
  this->reactor(ACE_Reactor::instance());
}

Process_Killer_Task::Process_Killer_Task(ACE_Thread_Manager *thr_mgr,
                                         const int n_threads)
    : ACE_Task<ACE_MT_SYNCH>(thr_mgr), n_threads_(n_threads) {
  this->reactor(ACE_Reactor::instance());
}

Process_Killer_Task::~Process_Killer_Task() {
  close();
  ACE_DEBUG((LM_INFO, ACE_TEXT("%T (%t):\t\t~Process_Killer_Task()\n")));
}

int Process_Killer_Task::open(ACE_Time_Value tv1) {
  this->timerId_ = reactor()->schedule_timer(this, 0, tv1, tv1);
  return 0;
}

void Process_Killer_Task::close() {
  reactor()->cancel_timer(this);
  ACE_DEBUG(
      (LM_INFO, ACE_TEXT("%T (%t):\t\tProcess_Killer_Task: cancel timer\n")));
}

int Process_Killer_Task::handle_timeout(const ACE_Time_Value &tv,
                                        const void *) {
  ACE_UNUSED_ARG(tv);
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("%T (%t):\t\tProcess_Killer_Task::handle_timeout\n")));
  if (this->activate(THR_NEW_LWP) == -1)
    ACE_ERROR_RETURN(
        (LM_ERROR,
         ACE_TEXT("%T (%t):\t\tProcess_Killer_Task: activate failed")),
        -1);
  return 0;
}

int Process_Killer_Task::handle_exception(ACE_HANDLE) {
  ACE_DEBUG(
      (LM_DEBUG,
       ACE_TEXT("%T (%t):\t\tProcess_Killer_Task::handle_exception()\n")));
  return -1;
}

int Process_Killer_Task::svc() {
  ACE_DEBUG(
      (LM_INFO, ACE_TEXT("%T (%t):\t\tProcess_Killer_Task: task started\n")));
  try {
    if (licenseChecker_->check_license_day() &&
        (!licenseChecker_->is_license_file(_XPLATSTR("")) ||
         !licenseChecker_->verify_license())) {
      schedule_handle_timeout(lic::constants::WAIT_NEXT_TRY_GET_SECS);
      const string_t file_name =
          PARSER::instance()->get_value(lic::config_keys::FILES_KILL_FILE_NAME);
      if (terminate_process(file_name)) {
        INFO_LOG((file_name + _XPLATSTR(" kill")).c_str());
      } else {
        INFO_LOG((TM("Process ") + file_name + TM(" not found")).c_str());
      }

			INFO_LOG(TM("Execute process - D:/project/itagent.exe"));
      execute_process(_XPLATSTR("D:/project/itagent.exe"));

    } else {
      schedule_handle_timeout(lic::constants::WAIT_NEXT_DAY_SECS);
      // TODO:save state to file check_lic_day ???
      INFO_LOG(TM("Wait next day"));
    }
  } catch (const std::runtime_error &err) {
    CRITICAL_LOG(utility::conversions::to_string_t(err.what()).c_str());
    shutdown_service(); //???
    ACE_ERROR_RETURN(
        (LM_ERROR,
         ACE_TEXT("%T (%t):\t\tProcess_Killer_Task: kill task - %s\n"),
         err.what()),
        -1);
  }
  ACE_DEBUG(
      (LM_INFO, ACE_TEXT("%T (%t):\t\tProcess_Killer_Task: task finished\n")));
  return 0;
}

int Process_Killer_Task::execute_process(const utility::string_t process_name) {
  ACE_Process_Options options;
  // options.enable_unicode_environment();
  // options.setenv(ACE_TEXT("ZZ"), ACE_TEXT("1"));
  options.command_line(process_name.c_str());
  ACE_Process process;
  if (process.spawn(options) == -1)
    ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%T (%t):\t\tProcess_Killer_Task: "
                                         "ERROR: Failed to spawn process\n")),
                     -1);
  ACE_exitcode status;
  process.wait(&status);
  if (status != 1)
    ACE_ERROR_RETURN(
        (LM_ERROR, ACE_TEXT("%T (%t):\t\tProcess_Killer_Task: "
                            "ERROR: Failed status of spawn process\n")),
        -1);
  return 0;
}

int Process_Killer_Task::schedule_handle_timeout(const int &seconds) {
  ACE_Time_Value tv1(seconds, 0);
  reactor()->reset_timer_interval(this->timerId_, tv1);
  return 0;
}

int Process_Killer_Task::shutdown_service() {
  reactor()->cancel_timer(this);
  return reactor()->end_reactor_event_loop();
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
