#include "notificator_linux.h"
#include "event_sink_task.h"
#include "nxsvc.h"

LinuxNoficitator::LinuxNoficitator() {
  //  ACE_Reactor r(new ACE_Dev_Poll_Reactor);
  //  this->reactor(&r);

  //  ACE_Dev_Poll_Reactor dev_poll_reactor_impl;
  //  ACE_Reactor dev_poll_reactor(&dev_poll_reactor_impl);
}

char *LinuxNoficitator::get_program_name_from_pid(const int pid, char *buffer,
                                                  const size_t buffer_size) {
  /* Try to get program name by PID */
  sprintf(buffer, "/proc/%d/cmdline", pid);
  const int fd = ::open(buffer, O_RDONLY);
  if (fd < 0) return nullptr;
  /* Read file contents into buffer */
  const ssize_t len = read(fd, buffer, buffer_size - 1);
  if (len <= 0) {
    close(fd);
    return nullptr;
  }
  close(fd);
  buffer[len] = '\0';
  char *aux = strstr(buffer, "^@");
  if (aux) *aux = '\0';
  return buffer;
}

string LinuxNoficitator::get_file_name_from_path(const char *buffer) {
  std::string file_path(buffer);
  const int pos = file_path.find_last_of(_XPLATSTR("/"));
  //  string file_name;
  if (pos != string_t::npos)
    return file_path.substr(pos + 1, file_path.length());
  //  printf("Received @@@@@@@@@@@@@@@@@@@@@@ '%s' '%d'", file_name.c_str(),
  //  pos);
  return nullptr;
}

// string LinuxNoficitator::get_file_name_from_path(const int fd, char *buffer,
//                                                 const size_t buffer_size) {
//  if (fd <= 0) return nullptr;
//  sprintf(buffer, "/proc/self/fd/%d", fd);
//  const ssize_t len = readlink(buffer, buffer, buffer_size - 1);
//  if (len < 0) return nullptr;
//  buffer[len] = '\0';

//  std::string file_path(buffer);
//  const int pos = file_path.find_last_of(_XPLATSTR("/"));
//  string file_name;
//  if (pos != string_t::npos)
//    file_name = file_path.substr(pos + 1, file_path.length());
//  //  printf("Received @@@@@@@@@@@@@@@@@@@@@@ '%s' '%d'", file_name.c_str(),
//  //  pos);
//  return file_name;
//}

int LinuxNoficitator::get_file_path_from_fd(const int fd, char *buffer,
                                            const size_t buffer_size) {
  if (fd <= 0) return -1;
  sprintf(buffer, "/proc/self/fd/%d", fd);
  const ssize_t len = readlink(buffer, buffer, buffer_size - 1);
  if (len < 0) return -1;
  buffer[len] = '\0';
  return 0;
}

// char *LinuxNoficitator::get_file_path_from_fd(const int fd, char *buffer,
//                                              const size_t buffer_size) {
//  if (fd <= 0) return nullptr;
//  sprintf(buffer, "/proc/self/fd/%d", fd);
//  const ssize_t len = readlink(buffer, buffer, buffer_size - 1);
//  if (len < 0) return nullptr;
//  buffer[len] = '\0';
//  return buffer;
//}

void LinuxNoficitator::event_process(
    const struct fanotify_event_metadata *event) {
  string file;
  char path[PATH_MAX];
  if (get_file_path_from_fd(event->fd, path, PATH_MAX) != -1)
    file = get_file_name_from_path(path);

  //  printf("Received event in path '%s'",
  //         get_file_path_from_fd(event->fd, path, PATH_MAX) ? path :
  //         "unknown");

  char buffer[512];
  int cx = snprintf(
      buffer, 512, "Received event in path '%s'",
      get_file_path_from_fd(event->fd, path, PATH_MAX) ? path : "unknown");
  DEBUG_LOG(buffer);

  //  const string file = get_file_name_from_path(event->fd, path, PATH_MAX);

  printf("File name is '%s' \n", file.c_str());

  printf(" pid=%d (%s): \n", event->pid,
         (get_program_name_from_pid(event->pid, path, PATH_MAX) ? path
                                                                : "unknown"));

  //  if (ACE_OS::strcmp(this->get_file_name().c_str(), event->name) == 0) {
  //    char_t buffer[BUFSIZ];
  //    const size_t len =
  //        ACE_OS::sprintf(buffer, "The %s was update\n", event->name);
  //    ACE_DEBUG(
  //        (LM_DEBUG, "%T Config_Handler::processing ", buffer, "(%t) \n"));
  //    INFO_LOG(buffer);
  //    ACE_OS::sleep(1);
  //    if (PARSER::instance()->init() == -1) {
  //      ACE_ERROR((LM_ERROR, "%T %p: cannot to initialize constants (%t)\n",
  //                 "\tConfig_Handler::handle_input"));
  //      raise(SIGINT);
  //    } else {
  //      SERVICE::instance()->reshedule_tasks();
  //    }
  //  }

  //  if (event->mask & FAN_ACCESS) {
  //    printf("\tFAN_ACCESS\n");
  //  }

  if (event->mask & FAN_OPEN) {
    //    LICENSE_WORKER_TASK::instance()->open();
    printf("\tFAN_OPEN\n");
  }

  if (event->mask & FAN_CLOSE_WRITE) {
    //      SERVICE::instance()->reshedule_tasks();
    printf("\tFAN_CLOSE_WRITE\n");
  }

  //  if (event->mask & FAN_MODIFY) {
  //    printf("\tFAN_MODIFY\n");
  //  }

  //  if (event->mask & FAN_ONDIR) {
  //    printf("\tFAN_ONDIR\n");
  //  }

  //  if (event->mask & FAN_EVENT_ON_CHILD) {
  //    printf("\tFAN_EVENT_ON_CHILD\n");
  //  }

  fflush(stdout);
  close(event->fd);
}

void LinuxNoficitator::shutdown_fanotify(const int numbers,
                                         const int fanotify_fd) {
  for (int i = 0; i < numbers; ++i) {
    /* Remove the mark, using same event mask as when creating it */
    fanotify_mark(fanotify_fd, FAN_MARK_REMOVE, event_mask, AT_FDCWD,
                  monitors[i].path);
    free(monitors[i].path);
  }
  free(monitors);
  close(fanotify_fd);
}

int LinuxNoficitator::initialize_fanotify(unsigned int numbers,
                                          const char **paths) {
  const int fanotify_fd = fanotify_init(FAN_CLOEXEC, init_mask);
  /* Create new fanotify device */
  if (fanotify_fd < 0) {
    fprintf(stderr, "Couldn't setup new fanotify device: %s\n",
            strerror(errno));
    return -1;
  }
  /* Allocate array of monitor setups */
  monitors = static_cast<monitored_t *>(malloc(numbers * sizeof(monitored_t)));
  /* Loop all input directories, setting up marks */
  for (unsigned int i = 0; i < numbers; ++i) {
    // monitors[i].path = strdup(paths[i + 1]);
    monitors[i].path = strdup(paths[i]);
    /* Add new fanotify mark */
    if (fanotify_mark(fanotify_fd, FAN_MARK_ADD, event_mask, AT_FDCWD,
                      monitors[i].path) < 0) {
      fprintf(stderr, "Couldn't add monitor in directory '%s': '%s'\n",
              monitors[i].path, strerror(errno));
      return -1;
    }
    printf("Started monitoring directory '%s'...\n", monitors[i].path);
  }
  return fanotify_fd;
}

int LinuxNoficitator::open(const ACE_Time_Value tv1) {
  //  // this->activate();
  //  numbers = static_cast<unsigned int>(argc);
  //  /* Initialize fanotify FD and the marks */
  //  fanotify_fd = initialize_fanotify(numbers, argv);

  //  if (fanotify_fd < 0) {
  //    fprintf(stderr, "Couldn't initialize fanotify\n");
  //    DEBUG_LOG(TM("Couldn't initialize fanotify"));
  //    exit(EXIT_FAILURE);
  //  }
  //  fds[FD_POLL_FANOTIFY].fd = fanotify_fd;
  //  fds[FD_POLL_FANOTIFY].events = POLLIN;

  return 0;
}

int LinuxNoficitator::close(u_long arg) { return 0; }

int LinuxNoficitator::handle_input(ACE_HANDLE) {
  printf("\tLinuxNoficitator::handle_inputLinuxNoficitator::handle_input\n");
  /* Block until there is something to be read */
  if (poll(fds, FD_POLL_MAX, -1) < 0) {
    char buffer[512];
    int cx = snprintf(buffer, 512, "Couldn't poll(): '%s'", strerror(errno));
    DEBUG_LOG(buffer);
    fprintf(stderr, "Couldn't poll(): '%s'\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  if (fds[FD_POLL_FANOTIFY].revents & POLLIN) {
    char buffer[FANOTIFY_BUFFER_SIZE];
    ssize_t length =
        read(fds[FD_POLL_FANOTIFY].fd, buffer, FANOTIFY_BUFFER_SIZE);
    if (length > 0) {
      struct fanotify_event_metadata *metadata;
      metadata = (struct fanotify_event_metadata *)buffer;
      while (FAN_EVENT_OK(metadata, length)) {
        event_process(metadata);
        if (metadata->fd > 0) close(metadata->fd);
        metadata = FAN_EVENT_NEXT(metadata, length);
      }
    }
  }
  return 0;
}

int LinuxNoficitator::svc() {
  /* Now loop */
  for (;;) {
    /* Block until there is something to be read */
    if (poll(fds, FD_POLL_MAX, -1) < 0) {
      char buffer[512];
      int cx = snprintf(buffer, 512, "Couldn't poll(): '%s'", strerror(errno));
      DEBUG_LOG(buffer);
      fprintf(stderr, "Couldn't poll(): '%s'\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    if (fds[FD_POLL_FANOTIFY].revents & POLLIN) {
      char buffer[FANOTIFY_BUFFER_SIZE];
      ssize_t length =
          read(fds[FD_POLL_FANOTIFY].fd, buffer, FANOTIFY_BUFFER_SIZE);
      if (length > 0) {
        struct fanotify_event_metadata *metadata;
        metadata = (struct fanotify_event_metadata *)buffer;
        while (FAN_EVENT_OK(metadata, length)) {
          event_process(metadata);
          if (metadata->fd > 0) close(metadata->fd);
          metadata = FAN_EVENT_NEXT(metadata, length);
        }
      }
    }
  }
  return 0;
}

int LinuxNoficitator::run_notify(int argc, const char *argv[]) {
  // int fanotify_fd;
  // struct pollfd fds[FD_POLL_MAX];
  /* Input arguments... */
  //  if (argc < 1) {
  //    fprintf(stderr, "Usage: %s directory1 [directory2 ...]\n", argv[0]);
  //    exit(EXIT_FAILURE);
  //  }

  numbers = static_cast<unsigned int>(argc);
  /* Initialize fanotify FD and the marks */
  fanotify_fd = initialize_fanotify(numbers, argv);

  if (fanotify_fd < 0) {
    fprintf(stderr, "Couldn't initialize fanotify\n");
    DEBUG_LOG(TM("Couldn't initialize fanotify"));
    exit(EXIT_FAILURE);
  }
  fds[FD_POLL_FANOTIFY].fd = fanotify_fd;
  fds[FD_POLL_FANOTIFY].events = POLLIN;

  //  if (this->reactor()->register_handler(this, ACE_Event_Handler::READ_MASK)
  //  ==
  //      -1)
  //    ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) Echo_Handler::open: %p\n"),
  //               ACE_TEXT("register_handler for input")));

  this->activate();
  return EXIT_SUCCESS;
}

LinuxNoficitator::~LinuxNoficitator() {
  // Clean exit
  shutdown_fanotify(static_cast<int>(numbers), fanotify_fd);
  printf("Exiting fanotify example...\n");
  DEBUG_LOG(TM("LinuxNoficitator::~LinuxNoficitator"));
}

ACE_HANDLE LinuxNoficitator::get_handle(void) const { return fanotify_fd; }
