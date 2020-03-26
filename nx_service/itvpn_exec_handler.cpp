#include "itvpn_exec_handler.h"
#include "ace/OS_NS_string.h"
#include "event_sink_task.h"
#include "parser_ini.h"
#include "tools.h"
#include "tracer.h"

namespace itvpnagent {

Itvpn_Exec_Handler::Itvpn_Exec_Handler(ACE_Reactor *r)
    : ACE_Event_Handler(r), handle_(ACE_INVALID_HANDLE) {
  // this->reactor(reactor);

  const std::string file_name = System::get_file_name_from_path(
      PARSER::instance()->options().openvpn_file_path.c_str());
  this->set_file_name(file_name);

  const std::string directory = System::get_path_without_file_name(
      PARSER::instance()->options().openvpn_file_path.c_str());
  this->set_directory(directory);

  char_t buffer[BUFSIZ];
  const size_t len =
      ACE_OS::sprintf(buffer, "Started monitoring file '%s'...\n",
                      PARSER::instance()->options().openvpn_file_path.c_str());
  ACE_DEBUG((LM_DEBUG, "%T Itvpn_Exec_Handler: %s ", buffer, "(%t) \n"));
  INFO_LOG(buffer);

  this->handle_ = inotify_init();
  if (this->handle_ == ACE_INVALID_HANDLE)
    ACE_ERROR((LM_ERROR, "Inotify handler could not be setup\n"));

  this->watch_ = inotify_add_watch(this->handle_, this->get_directory().c_str(), IN_OPEN);

  if (this->reactor()->register_handler(this, ACE_Event_Handler::READ_MASK) == -1)
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
      ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T Directory %s modify (%t)\n"), event->name));
    } else {
      if (event->mask & IN_OPEN) {
        if (ACE_OS::strcmp(this->get_file_name().c_str(), event->name) == 0) {
          char_t buffer[BUFSIZ];
          const size_t len =
              ACE_OS::sprintf(buffer, "The %s is opening...\n", event->name);
          ACE_DEBUG((LM_DEBUG, "%T Itvpn_Exec_Handler::processing ", buffer, "(%t) \n"));
          INFO_LOG(buffer);
          ACE_OS::sleep(1);
          LICENSE_WORKER_TASK::instance()->open();

          //
          this->reactor()->suspend_handler(this);
          // Remove for writing
          ACE_Reactor_Mask mask =
              ACE_Event_Handler::WRITE_MASK | ACE_Event_Handler::DONT_CALL;
          this->reactor()->remove_handler(this, mask);
          // event->signal();
        }
      }
    }
  }
  return 0;
}

int Itvpn_Exec_Handler::handle_signal(int, siginfo_t *, ucontext_t *) {
  ACE_DEBUG((LM_DEBUG, "(%t) Itvpn_Exec_Handler::handle_signal\n"));
  //
  this->reactor()->resume_handler(this);

  if (this->reactor()->register_handler(this, ACE_Event_Handler::READ_MASK) == -1)
    ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) Itvpn_Exec_Handler::open: %p\n"),
               ACE_TEXT("register_handler for input")));
  return 0;
}

int Itvpn_Exec_Handler::handle_close(ACE_HANDLE, ACE_Reactor_Mask) {
  ACE_DEBUG((LM_DEBUG, "Itvpn_Exec_Handler removed from Reactor\n"));
  return 0;
}

std::string Itvpn_Exec_Handler::get_directory() { return this->directory_; }

void Itvpn_Exec_Handler::set_directory(const std::string &directory) {
  this->directory_ = directory;
}

std::string Itvpn_Exec_Handler::get_file_name() { return this->file_name_; }

void Itvpn_Exec_Handler::set_file_name(const std::string &file_name) {
  this->file_name_ = file_name;
}

} // namespace itvpnagent
