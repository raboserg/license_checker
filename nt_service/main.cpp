#include "manager.h"

#if defined(ACE_WIN32) && !defined(ACE_LACKS_WIN32_SERVICES)

int ACE_TMAIN(int argc, ACE_TCHAR *argv[]) {
	//setlocale(LC_ALL, "ru_RU.UTF-8");
  return PROCESS::instance()->run(argc, argv);
}

#else

#include "ace/OS_main.h"

int ACE_TMAIN(int, ACE_TCHAR *[]) {
  // This program needs Windows services.
  return 0;
}

#endif /* ACE_WIN32 && !ACE_LACKS_WIN32_SERVICES */
