#pragma once

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
#include <unistd.h>
#include "tracer.h"

#include <map>
#include "ace/Dev_Poll_Reactor.h"
#include "ace/Reactor.h"
#include "ace/Task.h"

/* Size of buffer to use when reading fanotify events */
#define FANOTIFY_BUFFER_SIZE 8192

class LinuxNoficitator : public ACE_Task_Base {
  // class LinuxNoficitator : public ACE_Event_Handler {
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
       FAN_EVENT_ON_CHILD);  // | /**/
  // FAN_OPEN_EXEC);

  //  uint64_t event_mask = (FAN_ACCESS |      /* File accessed */
  //                         FAN_CLOSE_WRITE | /* Writtable file closed */
  //                         FAN_OPEN);        /* File was opened */

  struct pollfd fds[FD_POLL_MAX];

  std::map<std::string, std::string> monitored;

  unsigned int init_mask = (O_RDONLY | O_CLOEXEC | O_LARGEFILE);

  char *get_program_name_from_pid(const int pid, char *buffer,
                                  const size_t buffer_size);
  //  char *get_file_path_from_fd(const int fd, char *buffer,
  //                              const size_t buffer_size);
  int get_file_path_from_fd(const int fd, char *buffer,
                            const size_t buffer_size);

  std::string get_file_name_from_path(const char *buffer);

  void event_process(const struct fanotify_event_metadata *event);

  int initialize_fanotify(unsigned int numbers, const char **paths);

  // int fanotify_fd;
  ACE_HANDLE fanotify_fd;
  unsigned int numbers;
  virtual int svc(void);
  int handle_input(ACE_HANDLE fd);

 public:
  LinuxNoficitator();
  int close(u_long arg);
  virtual ~LinuxNoficitator();
  int open(const ACE_Time_Value tv1);
  ACE_HANDLE get_handle(void) const;
  int run_notify(int argc, const char *argv[]);
  void shutdown_fanotify(const int numbers, const int fanotify_fd);
};

static LinuxNoficitator theInstance;
