#ifndef LINUX_NOTIFICATOR_H
#define LINUX_NOTIFICATOR_H

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fanotify.h>
#include <sys/signalfd.h>
#include <sys/stat.h>
#include <tracer.h>
#include <unistd.h>

/* Size of buffer to use when reading fanotify events */
#define FANOTIFY_BUFFER_SIZE 8192

class LinuxNoficitator {
  /* Structure to keep track of monitored directories */
  typedef struct {
    /* Path of the directory */
    char *path;
  } monitored_t;
  /* Array of directories being monitored */
  monitored_t *monitors;
  /* Enumerate list of FDs to poll */
  enum { FD_POLL_FANOTIFY = 0, FD_POLL_SIGNAL, FD_POLL_MAX };
  // Setup fanotify notifications (FAN) mask. All these defined in fanotify.h.
  uint64_t event_mask =
      (FAN_ACCESS |        /* File accessed */
       FAN_MODIFY |        /* File modified */
       FAN_CLOSE_WRITE |   /* Writtable file closed */
       FAN_CLOSE_NOWRITE | /* Unwrittable file closed */
       FAN_OPEN |          /* File was opened */
       FAN_ONDIR | /* We want to be reported of events in the directory */
       FAN_EVENT_ON_CHILD | FAN_OPEN_EXEC);

  unsigned int init_mask = (O_RDONLY | O_CLOEXEC | O_LARGEFILE);

  char *get_program_name_from_pid(const int pid, char *buffer,
                                  const size_t buffer_size);
  char *get_file_path_from_fd(const int fd, char *buffer,
                              const size_t buffer_size);
  void event_process(const struct fanotify_event_metadata *event);

  int initialize_fanotify(unsigned int numbers, char **paths);

  int fanotify_fd;
  unsigned int numbers;

public:
  virtual ~LinuxNoficitator();
  int run_notify(int argc, char *argv[]);
  void shutdown_fanotify(const int numbers, const int fanotify_fd);
  static void srv_run(void *argc);
};

static LinuxNoficitator theInstance;
#endif // LINUX_NOTIFICATOR_H
