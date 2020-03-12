#include "config_change.h"
#include "parser_ini.h"

Config_Handler::Config_Handler(ACE_Reactor *reactor)
    : handle_(ACE_INVALID_HANDLE), event_(new ACE_Auto_Event), bStop(false) {
  this->reactor(reactor);
  this->open();
}

Config_Handler::~Config_Handler(void) {
  CloseHandle(this->handle_);
  this->reactor(0);
  delete[] this->event_;
}

ACE_HANDLE
Config_Handler::get_handle(void) const { return this->handle_; }

int Config_Handler::open() {
  this->handle_ = ::CreateFile(
      directory, GENERIC_READ,
      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
      OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

  if (this->handle_ == ACE_INVALID_HANDLE)
    ACE_ERROR_RETURN((LM_ERROR, "Config_Handler::open(): could not be setup\n"),
                     -1);

  if (this->reactor()->register_handler(this, this->event_->handle()) != 0)
    ACE_ERROR((LM_ERROR, "Registration with Reactor could not be done\n"));
  this->activate();
  return 0;
}

int Config_Handler::svc(void) {
  ACE_DEBUG((LM_DEBUG, "Config_Handler::svc\n"));
  OVERLAPPED o = {};
  o.hEvent = CreateEvent(0, FALSE, FALSE, 0); // event_->handle();
  DWORD nBufferLength = 60 * 1024;
  BYTE *lpBuffer = new BYTE[nBufferLength];
  while (!bStop) {
    DWORD returnedBytes = 0;
    DWORD notifyFilter = FILE_NOTIFY_CHANGE_LAST_WRITE;
    ::ReadDirectoryChangesW(this->handle_, lpBuffer, nBufferLength, TRUE,
                            notifyFilter, &returnedBytes, &o, 0);
    DWORD dwWaitStatus = WaitForSingleObject(o.hEvent, INFINITE);
    switch (dwWaitStatus) {
    case WAIT_OBJECT_0: {
      DWORD seek = 0;
      while (seek < nBufferLength) {
        PFILE_NOTIFY_INFORMATION pNotify =
            PFILE_NOTIFY_INFORMATION(lpBuffer + seek);
        seek += pNotify->NextEntryOffset;
        const ULONG ulCount = pNotify->FileNameLength / 2;
        WCHAR szwFileName[MAX_PATH];
        wcsncpy(szwFileName, pNotify->FileName, ulCount);
        szwFileName[ulCount] = L'\0';
        if (ACE_OS::strcmp(szwFileName, temp_file) == 0) {
          wprintf(L"%d. %s\n", pNotify->Action, szwFileName);
          event_->signal();
          break;
        }
        if (pNotify->NextEntryOffset == 0)
          break;
      } // while
    } break;
    default:
      bStop = true;
      break;
    } // switch
  }   // while
  CloseHandle(o.hEvent);
  delete[] lpBuffer;
  return 0;
}

int Config_Handler::handle_signal(int, siginfo_t *, ucontext_t *) {
  ACE_DEBUG((LM_DEBUG, "Config_Handler::handle_signal\n"));
  if (PARSER::instance()->init() == -1) {
    ACE_ERROR((LM_ERROR, "%T (%t) cannot to initialize constants\n",
               "\tService::svc"));
    ACE_OS::sleep(3);
    raise(SIGINT);
  }
  return 0;
}

int Config_Handler::handle_close(ACE_HANDLE, ACE_Reactor_Mask) {
  ACE_DEBUG((LM_DEBUG, "Config_Handler removed from Reactor\n"));
  return 0;
}
