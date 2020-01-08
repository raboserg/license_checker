#ifdef _WIN32
#include "notificator_windows.h"
#include "windows.h"
#else
#include "notificator_linux.h"
#endif

#ifdef _WIN32
void Work_Proc(void *arg) {
  HANDLE cencel_event = (HANDLE)arg;
  WinNT::Notificator notificator_(L"openvpn.exe");
  notificator_.notification_wait(cencel_event);
  _endthread();
}

static void test_notification() {
  HANDLE cencel_event = CreateEvent(NULL, TRUE, FALSE, L"CencelEvent");
  HANDLE hThread = (HANDLE)_beginthread(&Work_Proc, 1024, cencel_event);
  Sleep(10000);
  SetEvent(cencel_event);
  Sleep(10000);
}

int main(int argc, char **argv) {
	/*HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);*/
	test_notification();
  return 0;
}
#else
main_run();
#endif
