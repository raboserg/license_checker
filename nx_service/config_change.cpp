#include "config_change.h"
#include "ace/OS_NS_string.h"
#include "nxsvc.h"
#include "parser_ini.h"
#include "tools.h"

namespace itvpnagent {

Config_Handler::Config_Handler(ACE_Reactor *r)
    : ACE_Event_Handler(r), handle_(ACE_INVALID_HANDLE), iterations_(0) {

  this->set_file_name(PARSER::instance()->get_config_file_name());
  const string directory = PARSER::instance()->get_service_path();
  this->set_directory(directory);

  std::array<char_t, BUFSIZ> buffer;
  const size_t len = ACE_OS::sprintf(
      buffer.data(), "Started monitoring file '%s'", directory.c_str());
  ACE_DEBUG((LM_DEBUG, "%T Config_Handler: %s ", buffer.data(), "(%t) \n"));
  INFO_LOG(buffer.data());

  this->handle_ = inotify_init();
  this->watch_ = inotify_add_watch(this->handle_, this->get_directory().c_str(),
                                   IN_MODIFY);

  if (this->handle_ == ACE_INVALID_HANDLE)
    ACE_ERROR((LM_ERROR, "FindFirstChangeNotification could not be setup\n"));

  if (this->reactor()->register_handler(this, ACE_Event_Handler::READ_MASK) ==
      -1)
    ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) Echo_Handler::open: %p\n"),
               ACE_TEXT("register_handler for input")));
}

Config_Handler::~Config_Handler(void) {
  ACE_DEBUG((LM_DEBUG, "(%t) Config_Handler::~Config_Handler\n"));
  inotify_rm_watch(this->handle_, watch_);
}

ACE_HANDLE Config_Handler::get_handle(void) const { return handle_; }

int Config_Handler::handle_input(ACE_HANDLE) {
  ACE_DEBUG((LM_DEBUG, "%T Config_Handler::handle_input (%t) \n"));
  char buffer[EVENT_BUF_LEN];
  if (read(this->handle_, buffer, EVENT_BUF_LEN) < 0)
    ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("length < 0")), -1);
  const struct inotify_event *event = (struct inotify_event *)&buffer;
  if (event->len) {
    if (event->mask & IN_ISDIR) {
      ACE_DEBUG(
          (LM_DEBUG, ACE_TEXT("%T Directory %s modify (%t)\n"), event->name));
    } else {
      if (event->mask & IN_MODIFY) {
        if (ACE_OS::strcmp(this->get_file_name().c_str(), event->name) == 0) {
          if (iterations_++ > 1) {
            iterations_ = 0;
            return 0;
          }
          // char_t buffer[BUFSIZ];
          std::array<char_t, BUFSIZ> buffer;
          const size_t len = ACE_OS::sprintf(
              buffer.data(), "The %s was update\n", event->name);
          ACE_DEBUG((LM_DEBUG, "%T Config_Handler::processing ", buffer.data(),
                     "(%t) \n"));
          INFO_LOG(buffer.data());
          ACE_OS::sleep(1);
          if (PARSER::instance()->init() == -1) {
            ACE_ERROR((LM_ERROR,
                       "%T %p: Cannot to initialize constants (%t) \n",
                       "\tConfig_Handler::handle_input"));
            raise(SIGINT);
          } else {
            SERVICE::instance()->reshedule_tasks();
          }
        }
      }
    }
  }
  return 0;
}

int Config_Handler::handle_close(ACE_HANDLE, ACE_Reactor_Mask) {
  ACE_DEBUG((LM_DEBUG, "Config_Handler removed from Reactor\n"));
  return 0;
}

std::string Config_Handler::get_directory() const { return this->directory_; }

void Config_Handler::set_directory(const std::string &directory) {
  this->directory_ = directory;
}

std::string Config_Handler::get_file_name() const { return this->file_name_; }

void Config_Handler::set_file_name(const std::string &file_name) {
  this->file_name_ = file_name;
}

} // namespace itvpnagent
