#include "config_change.h"
#include "ace/OS_NS_string.h"
#include "parser_ini.h"

Config_Handler::Config_Handler(ACE_Reactor *reactor)
    : handle_(ACE_INVALID_HANDLE) /*,
       done_handler_(ACE_Sig_Handler_Ex(ACE_Reactor::end_event_loop))*/
{
  this->reactor(reactor);

  //  if (this->reactor()->register_handler(SIGINT, &this->done_handler_) == -1)
  //  {
  //    ACE_ERROR(
  //        (LM_ERROR, "%T %p:\tcannot to register_handler SIGINT\t (%t) \n"));
  //    this->reactor()->notify(this, ACE_Event_Handler::EXCEPT_MASK);
  //  }

  this->handle_ = inotify_init();
  wd_ = inotify_add_watch(this->handle_, directory, IN_MODIFY);

  if (this->handle_ == ACE_INVALID_HANDLE)
    ACE_ERROR((LM_ERROR, "FindFirstChangeNotification could not be setup\n"));

  if (this->reactor()->register_handler(this, ACE_Event_Handler::READ_MASK) ==
      -1)
    ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) Echo_Handler::open: %p\n"),
               ACE_TEXT("register_handler for input")));
}

Config_Handler::~Config_Handler(void) {
  ACE_DEBUG((LM_DEBUG, "(%t) Event_Handler::~Event_Handler\n"));
  inotify_rm_watch(this->handle_, wd_);
}

ACE_HANDLE Config_Handler::get_handle(void) const { return handle_; }

int Config_Handler::handle_input(ACE_HANDLE) {
  ACE_DEBUG((LM_DEBUG, "(%t) Event_Handler::handle_input\n"));
  char buffer[EVENT_BUF_LEN];
  if (read(this->handle_, buffer, EVENT_BUF_LEN) < 0)
    ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("length < 0")), -1);
  struct inotify_event *event = (struct inotify_event *)&buffer;
  if (event->len) {
    if (event->mask & IN_MODIFY) {
      if (event->mask & IN_ISDIR) {
        printf("Directory %s modify.\n", event->name);
      } else {
        const char *sfdf = PARSER::instance()->get_config_file_name().c_str();
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Event_Handler: File %s modify.\n"),
                   event->name));
        if (ACE_OS::strcmp(sfdf, event->name) == 0) {
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("(%t) Event_Handler: ACE_OS::strcmp(sfdf, "
                              "event->name) == 0).\n"),
                     event->name));
          if (PARSER::instance()->init() == -1) {
            ACE_ERROR((LM_ERROR, "%T (%t) %p: cannot to initialize constants\n",
                       "\tConfig_Handler::handle_input"));
            raise(SIGINT);
          }
        }
      }
    }
  }
  return 0;
}

int Config_Handler::handle_signal(int, siginfo_t *, ucontext_t *) {
  ACE_DEBUG((LM_DEBUG, "(%t) Event_Handler::handle_signal\n"));
  return 0;
}

int Config_Handler::handle_close(ACE_HANDLE, ACE_Reactor_Mask) {
  ACE_DEBUG((LM_DEBUG, "Event_Handler removed from Reactor\n"));
  return 0;
}
