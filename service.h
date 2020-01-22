#pragma once
#include "notificator_win.h"
#include <stdio.h>
#include <tchar.h>
#include <windows.h>

namespace WinNT {

const LPCWSTR SERVICE_NAME = TEXT("Lisence checker");
HANDLE hStopEvent;
// Thread handle...
HANDLE hThread = NULL;
LPTSTR lpszServiceName;
SERVICE_STATUS_HANDLE ssh;

unsigned WINAPI Working_Proc(void *lpParameter);
// Service start
void WINAPI Service_Main(DWORD dwArgc, LPTSTR *lpszArgv);
// Service control
void WINAPI Service_Ctrl(DWORD dwCtrlCode);
// Stop service
void ErrorStopService(LPTSTR lpszAPI);
void SetTheServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode,
                         DWORD dwCheckPoint, DWORD dwWaitHint);

int Start_Service() {
  SERVICE_TABLE_ENTRY ste[] = {
      {(LPWSTR)SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)Service_Main},
      {NULL, NULL}};
  if (!StartServiceCtrlDispatcher(ste)) {
    WinNT::SvcDebugOut(
        TEXT("Error code for StartServiceCtrlDispatcher(): %u.\n"),
        GetLastError());
  } else
    OutputDebugString(TEXT("StartServiceCtrlDispatcher() returned!\n"));
  return 0;
}

// Called by the service control manager after the call to
// StartServiceCtrlDispatcher()...
void WINAPI Service_Main(DWORD dwArgc, LPTSTR *lpszArgv) {
  DWORD dwWaitRes;
  unsigned int ThreadId;
  // Obtain the name of the service...
  lpszServiceName = lpszArgv[0];
  // Register the service ctrl handler...
  ssh = RegisterServiceCtrlHandler(lpszServiceName,
                                   (LPHANDLER_FUNCTION)Service_Ctrl);
  // Create the event to signal the service to stop...
  hStopEvent = CreateEvent(NULL, TRUE, FALSE, SERVICE_NAME);
  if (hStopEvent == NULL)
    ErrorStopService((LPWSTR) "CreateEvent()");
  else
    printf("CreateEvent() looks OK.\n");
  hThread = (HANDLE)_beginthreadex(NULL, 0, &Working_Proc, NULL, 0, &ThreadId);
  if (hThread == INVALID_HANDLE_VALUE)
    ErrorStopService((LPTSTR) "CreateThread()");
  else
    printf("CreateThread() looks OK.\n");
  printf("Thread ID: %lu.\n", ThreadId);
  // The service has started...
  SetTheServiceStatus(SERVICE_RUNNING, 0, 0, 0);
  OutputDebugString(TEXT("SetTheServiceStatus(), SERVICE_RUNNING.\n"));
  // Main loop for the service...
  while (WaitForSingleObject(hStopEvent, 1000) != WAIT_OBJECT_0) {
    /***************************************************************/
    // Main loop for service.
    /***************************************************************/
  }
  for (INT t = 1; TRUE; t++) {
    if ((dwWaitRes = WaitForSingleObject(hThread, 1000)) == WAIT_OBJECT_0)
      break;
    else if ((dwWaitRes == WAIT_FAILED) || (dwWaitRes == WAIT_ABANDONED))
      ErrorStopService((LPTSTR) "WaitForMultipleObjects()");
    else
      SetTheServiceStatus(SERVICE_STOP_PENDING, 0, t, 3000);
  }
  // Close the event handle and the thread handle.
  if (!CloseHandle(hStopEvent))
    ErrorStopService((LPTSTR) "CloseHandle()");
  if (!CloseHandle(hThread))
    ErrorStopService((LPTSTR) "CloseHandle()");
  // Stop the service.
  OutputDebugString(TEXT("SetTheServiceStatus(), SERVICE_STOPPED.\n"));
  SetTheServiceStatus(SERVICE_STOPPED, NO_ERROR, 0, 0);
}
// Handles control signals from the service control manager...
// WINAPI - using __stdcall convention explicitly instead of __cdecl...
void WINAPI Service_Ctrl(DWORD dwCtrlCode) {
  DWORD dwState = SERVICE_RUNNING;
  switch (dwCtrlCode) {
  case SERVICE_CONTROL_STOP:
    dwState = SERVICE_STOP_PENDING;
    break;
  case SERVICE_CONTROL_SHUTDOWN:
    dwState = SERVICE_STOP_PENDING;
    break;
  case SERVICE_CONTROL_INTERROGATE:
    break;
  default:
    break;
  }
  // Set the status of the service.
  SetTheServiceStatus(dwState, NO_ERROR, 0, 0);
  OutputDebugString(TEXT("SetTheServiceStatus(), Service_Ctrl() function\n"));
  // Tell Service_Main thread to stop...
  if ((dwCtrlCode == SERVICE_CONTROL_STOP) ||
      (dwCtrlCode == SERVICE_CONTROL_SHUTDOWN)) {
    if (!SetEvent(hStopEvent))
      ErrorStopService((LPTSTR) "SetEvent()");
    else
      OutputDebugString(TEXT("Signal Service_Main() thread.\n"));
  }
}

// Thread procedure for all five worker threads...
unsigned WINAPI Working_Proc(void *lpParameter) {
  //TCHAR szOutput[25];
  OutputDebugString(TEXT("Working_Proc() created!\n"));
  int nThreadNum = reinterpret_cast<int>(lpParameter);
  Notificator notificator_(L"openvpn.exe");
  notificator_.notification_wait(hStopEvent);
  _endthreadex(0);
  return 0;
}
//  Wraps SetServiceStatus()...
void SetTheServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode,
                         DWORD dwCheckPoint, DWORD dwWaitHint) {
  // Current status of the service.
  SERVICE_STATUS service_status;
  // Disable control requests until the service is started.
  if (dwCurrentState == SERVICE_START_PENDING)
    service_status.dwControlsAccepted = 0;
  else
    service_status.dwControlsAccepted =
        SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
  // Other flags include SERVICE_ACCEPT_PAUSE_CONTINUE
  // and SERVICE_ACCEPT_SHUTDOWN.
  // Initialize ss structure.
  service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
  service_status.dwServiceSpecificExitCode = 0;
  service_status.dwCurrentState = dwCurrentState;
  service_status.dwWin32ExitCode = dwWin32ExitCode;
  service_status.dwCheckPoint = dwCheckPoint;
  service_status.dwWaitHint = dwWaitHint;
  // Send status of the service to the Service Controller.
  if (!SetServiceStatus(ssh, &service_status))
    ErrorStopService((LPTSTR) "SetServiceStatus()");
  else
    printf("SetServiceStatus() looks OK.\n");
  return;
}

//  Handle API errors or other problems by ending the service and
//  displaying an error message to the debugger.
void ErrorStopService(LPTSTR lpszAPI) {
  DWORD dwWaitRes;
  LPVOID lpvMessageBuffer;
  TCHAR buffer[256] = TEXT("");
  TCHAR error[1024] = TEXT("");

  wsprintf(buffer, TEXT("API = %s, "), lpszAPI);
  lstrcat(error, buffer);
  ZeroMemory(buffer, sizeof(buffer));
  wsprintf(buffer, TEXT("error code = %d, "), GetLastError());
  lstrcat(error, buffer);
  // Obtain the error string.
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR)&lpvMessageBuffer, 0, NULL);
  ZeroMemory((LPVOID)buffer, (DWORD)sizeof(buffer));
  wsprintf(buffer, TEXT("message = %s"), (TCHAR *)lpvMessageBuffer);
  lstrcat(error, buffer);
  // Free the buffer allocated by the system.
  LocalFree(lpvMessageBuffer);
  // Write the error string to the debugger.
  OutputDebugString(error);
  // If you have threads running, tell them to stop. Something went
  // wrong, and you need to stop them so you can inform the SCM.
  SetEvent(hStopEvent);
  // Wait for the threads to stop.
  for (INT t = 1; TRUE; t++) {
    if ((dwWaitRes = WaitForSingleObject(hThread, 1000)) == WAIT_OBJECT_0)
      break;
    else if ((dwWaitRes == WAIT_FAILED) || (dwWaitRes == WAIT_ABANDONED))
      break; // Our wait failed
    else
      SetTheServiceStatus(SERVICE_STOP_PENDING, 0, t, 3000);
  }
  // Stop the service.
  SetTheServiceStatus(SERVICE_STOPPED, GetLastError(), 0, 0);
  return;
}
} // namespace WinNT
