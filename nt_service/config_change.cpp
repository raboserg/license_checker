#include "config_change.h"
#include "parser_ini.h"
#include "tracer.h"

#define FILE_SHARE_MODE FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE
#define DISPOSITION FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED

Config_Handler::Config_Handler(ACE_Reactor *reactor)
    : handle_(ACE_INVALID_HANDLE), event_(new ACE_Auto_Event),
      active_handler_(new ACE_Auto_Event), bStop(false) {
  this->reactor(reactor);
  this->open();
}

Config_Handler::~Config_Handler(void) {
  ACE_DEBUG((LM_DEBUG, "Config_Handler::~Config_Handler\n"));
  this->bStop = true;
  // CloseHandle(this->handle_);
  this->reactor(0);
  delete[] this->event_;
}

int Config_Handler::open() {
  this->set_directory(
      ACE_WString(PARSER::instance()->get_service_path().c_str()));
  this->set_file_name(
      ACE_WString(PARSER::instance()->get_config_file_name().c_str()));
  if (this->reactor()->register_handler(this, this->event_->handle()) != 0)
    ACE_ERROR((LM_ERROR, "Registration with Reactor could not be done\n"));
  this->activate();
  return 0;
}

int Config_Handler::handle_timeout(const ACE_Time_Value &tv, const void *arg) {
  ACE_DEBUG((LM_DEBUG, "Config_Handler::handle_timeout\n"));
  return 0;
}

int Config_Handler::create_file() {
  this->handle_ =
      CreateFile(this->get_directory().c_str(), GENERIC_READ, FILE_SHARE_MODE,
                 NULL, OPEN_EXISTING, DISPOSITION, NULL);
  if (this->handle_ == ACE_INVALID_HANDLE) {
    char_t buffer[BUFSIZ];
    const size_t len = ACE_OS::sprintf(
        buffer,
        _XPLATSTR(
            "%T \tConfig_Handler::svc: could not be setup changes in %s \n"),
        this->get_directory());
    ERROR_LOG(buffer);
    ACE_ERROR_RETURN((LM_ERROR, buffer), -1);
  }
  return 0;
}

int Config_Handler::svc(void) {
  ACE_DEBUG((LM_DEBUG, "%T Config_Handler::svc (%t)\n"));
  OVERLAPPED o = {};
  o.hEvent = CreateEvent(0, FALSE, FALSE, 0); // event_->handle();
  DWORD nBufferLength = 60 * 1024;
  // TODO Use gls for pointer
  this->bStop = false;
  while (!bStop) {
    if (create_file() == -1) {
      this->bStop = true;
      break;
    }
    BYTE *lpBuffer = new BYTE[nBufferLength];
    DWORD returnedBytes = 0;
    ReadDirectoryChangesW(this->handle_, lpBuffer, nBufferLength, TRUE,
                          FILE_NOTIFY_CHANGE_LAST_WRITE, &returnedBytes, &o, 0);
    switch (WaitForSingleObject(o.hEvent, INFINITE)) {
    case WAIT_OBJECT_0: {
      processing(lpBuffer, nBufferLength);
      delete[] lpBuffer;
    } break;
    default:
      this->bStop = true;
      break;
    }
  }
  CloseHandle(o.hEvent);
  active_handler_->signal();
  return 0;
}

int Config_Handler::handle_signal(int, siginfo_t *, ucontext_t *) {
  ACE_DEBUG((LM_DEBUG, "%T Config_Handler::handle_signal (%t) \n"));
  // ACE_OS::sleep(1);
  active_handler_->wait();
  if (PARSER::instance()->init() == -1) {
    ACE_ERROR((LM_ERROR, "%T \tConfig_Handler::handle_signal: Cannot to "
                         "initialize constants (%t) \n"));
    INFO_LOG(TM("Shutting down service"));
    // SEND ERROR MESSAGE FOR INPUT CORRECT VALUE
    raise(SIGINT); //???
    return -1;
  }
  this->activate();
  return 0;
}

int Config_Handler::processing(const BYTE *lpBuffer,
                               const DWORD nBufferLength) {
  DWORD seek = 0;
  while (seek < nBufferLength) {
    PFILE_NOTIFY_INFORMATION pNotify =
        PFILE_NOTIFY_INFORMATION(lpBuffer + seek);
    if (pNotify->Action > 5)
      break;
    seek += pNotify->NextEntryOffset;
    const ULONG ulCount = pNotify->FileNameLength / 2;
    WCHAR szwFileName[MAX_PATH];
    wcsncpy(szwFileName, pNotify->FileName, ulCount);
    szwFileName[ulCount] = L'\0';
    if (ACE_OS::strcmp(szwFileName, this->get_file_name().c_str()) == 0) {
      char_t buffer[BUFSIZ];
      const size_t len = ACE_OS::sprintf(buffer, L"%d. %s was update\n",
                                         pNotify->Action, szwFileName);
      ACE_DEBUG(
          (LM_DEBUG, "%T Config_Handler::processing ", buffer, "(%t) \n"));
      INFO_LOG(buffer);
      this->event_->signal();
      this->bStop = true;
      CloseHandle(this->handle_);
      break;
    }
    if (pNotify->NextEntryOffset == 0)
      break;
  }
  return 0;
}

int Config_Handler::handle_close(ACE_HANDLE, ACE_Reactor_Mask) {
  ACE_DEBUG((LM_DEBUG, "%T Config_Handler removed from Reactor (%t) \n"));
  return 0;
}

ACE_WString Config_Handler::get_directory() { return directory_; }

void Config_Handler::set_directory(ACE_WString &directory) {
  this->directory_ = directory;
}

ACE_WString Config_Handler::get_file_name() { return this->file_name_; }

void Config_Handler::set_file_name(ACE_WString &file_name) {
  this->file_name_ = file_name;
}

ACE_HANDLE
Config_Handler::get_handle(void) const { return this->handle_; }