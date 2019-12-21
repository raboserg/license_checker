#include "exec_notificator.h"
#include <thread>


static void test_notification() {
  exec_notificator exec_notificator_(L"openvpn.exe");
  HANDLE cencel_event = CreateEvent(NULL, TRUE, FALSE, L"CencelEvent");
  std::thread(exec_notificator_, cencel_event).detach(); 
  Sleep(10000);
  SetEvent(cencel_event);
  Sleep(10000);
}

int main(int argc, char **argv) {
  test_notification();
  return 0; // Program successfully completed.
}