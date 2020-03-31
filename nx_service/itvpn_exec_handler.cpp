#include "itvpn_exec_handler.h"
#include "ace/OS_NS_string.h"
#include "event_sink_task.h"
#include "parser_ini.h"
#include "tools.h"
#include "tracer.h"

namespace itvpnagent {

Itvpn_Exec_Handler::Itvpn_Exec_Handler(ACE_Reactor *r)
    : ACE_Event_Handler(r), handle_(ACE_INVALID_HANDLE), iterations_(0) {

  const string openvpn_file_path = CONFIG.openvpn_file_path;

  const string file_name = Files::split_file_name(openvpn_file_path.c_str());
  this->set_file_name(file_name);

  const string directory = Files::split_file_path(openvpn_file_path.c_str());
  this->set_directory(directory);

  std::array<char_t, BUFSIZ> buffer;
  const size_t len = ACE_OS::sprintf(
      buffer.data(), "Started monitoring file '%s'", openvpn_file_path.c_str());
  ACE_DEBUG((LM_DEBUG, "%T Itvpn_Exec_Handler: %s ", buffer.data(), "(%t)\n"));
  INFO_LOG(buffer.data());

  this->handle_ = inotify_init();
  if (this->handle_ == ACE_INVALID_HANDLE)
    ACE_ERROR((LM_ERROR, "Inotify handler could not be setup\n"));

  this->watch_ =
      inotify_add_watch(this->handle_, this->get_directory().c_str(), IN_OPEN);

  if (this->reactor()->register_handler(this, ACE_Event_Handler::READ_MASK) ==
      -1)
    ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) Itvpn_Exec_Handler::open: %p\n"),
               ACE_TEXT("register_handler for input")));
}

Itvpn_Exec_Handler::~Itvpn_Exec_Handler(void) {
  ACE_DEBUG((LM_DEBUG, "(%t) Itvpn_Exec_Handler::~Itvpn_Exec_Handler\n"));
  inotify_rm_watch(this->handle_, watch_);
}

ACE_HANDLE Itvpn_Exec_Handler::get_handle(void) const { return handle_; }

int Itvpn_Exec_Handler::handle_input(ACE_HANDLE) {
  ACE_DEBUG((LM_DEBUG, "%T Itvpn_Exec_Handler::handle_input (%t) \n"));
  char buffer[EVENT_BUF_LEN];
  if (read(this->handle_, buffer, EVENT_BUF_LEN) < 0)
    ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("length < 0")), -1);
  const struct inotify_event *event = (struct inotify_event *)&buffer;
  if (event->len) {
    if (event->mask & IN_ISDIR) {
      ACE_DEBUG(
          (LM_DEBUG, ACE_TEXT("%T Directory %s modify (%t)\n"), event->name));
    } else {
      if (event->mask & IN_OPEN) {
        if (ACE_OS::strcmp(this->get_file_name().c_str(), event->name) == 0) {
          if (iterations_++ > 1) {
            iterations_ = 0;
            return 0;
          }
          std::array<char_t, BUFSIZ> buffer;
          const size_t len = ACE_OS::sprintf(
              buffer.data(), "The %s is opening...\n", event->name);
          ACE_DEBUG((LM_DEBUG, "%T Itvpn_Exec_Handler::processing ",
                     buffer.data(), "(%t) \n"));
          INFO_LOG(buffer.data());
          LICENSE_WORKER_TASK::instance()->open();
        }
      }
    }
  }
  return 0;
}

int Itvpn_Exec_Handler::handle_close(ACE_HANDLE, ACE_Reactor_Mask) {
  ACE_DEBUG((LM_DEBUG, "Itvpn_Exec_Handler removed from Reactor\n"));
  return 0;
}

string Itvpn_Exec_Handler::get_directory() const { return this->directory_; }

void Itvpn_Exec_Handler::set_directory(const string &directory) {
  this->directory_ = directory;
}

string Itvpn_Exec_Handler::get_file_name() const { return this->file_name_; }

void Itvpn_Exec_Handler::set_file_name(const string &file_name) {
  this->file_name_ = file_name;
}

} // namespace itvpnagent
