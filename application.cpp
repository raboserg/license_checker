#include "stdafx.h"
#include <signal.h>

//#define TEST_TRACE(X) ACE_Trace ____(ACE_TEXT(X), __LINE__,
//ACE_TEXT(__FILE__))

enum {
  error_create_lic = 2, // mybe create process
};

// utility::string_t input_handle();

void posix_death_signal(int signum) {
  //  const std::error_category fdsfsd = utility::details::linux_category();
  //  std::cout << fdsfsd << std::endl;
  CRITICAL_LOG(TM("ABORTED"));
  signal(signum, SIG_DFL); // resend signal
  // ucout << input_handle();
  // lic::os_utilities::sleep(1000);
  exit(3);
}

int main(int argc, char *argv[]) {

#ifdef _WIN32
  return PROCESS::instance()->run(argc, argv);
#else
  LinuxNoficitator linuxNoficitator_;
  linuxNoficitator_.run_notify(argc, argv);
#endif

  // setlocale(LC_ALL, "ru_RU.UTF-8");
  // signal(SIGSEGV, posix_death_signal);
  // lic::os_utilities::sleep(1000);
  // ucout << input_handle();
  return 0;
}

// utility::string_t input_handle() {
//  utility::string_t cmd;
//  ucout << U("Input <Enter> for close") << std::endl;
//  getline(ucin, cmd);
//  return cmd;
//}
