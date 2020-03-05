#ifdef _WIN32
#include "initializer.h"
#else
#include <nxsvc.h>
#endif

#include <constants.h>
#include <tracer.h>

int main(int argc, char *argv[]) {
  // test_event();
#ifdef _WIN32
  return PROCESS::instance()->run(argc, argv);
  // setlocale(LC_ALL, "ru_RU.UTF-8");
#else
  return SERVICE::instance()->run();
#endif

  return 0;
}
