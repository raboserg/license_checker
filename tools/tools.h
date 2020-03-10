#pragma once
#ifndef TOOLS_H
#define TOOLS_H

#ifdef _WIN32
#include <Windows.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include <dirent.h>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/types.h>
#endif
//#include <cpprest/details/basic_types.h>
#include <memory>

namespace utils {

struct ip_helper {

  //  static int get_hostname(char *hostbuffer, int size) {
  //    return gethostname(hostbuffer, size);
  //  }
  //
  //  static hostent *get_hostbyname(char *hostbuffer) {
  //    struct hostent *host_entry = nullptr;
  //    host_entry = gethostbyname(hostbuffer);
  //    return host_entry;
  //  }
  //
  //  static std::unique_ptr<char *> get_ip() {
  //    char buf[128];
  //    std::unique_ptr<char *> ip_buffer = nullptr;
  //#ifdef _WIN32
  //    WSAData wsaData;
  //    const int WSVer = MAKEWORD(2, 2);
  //    if (WSAStartup(WSVer, &wsaData) == 0) {
  //#endif
  //      if (get_hostname(buf, sizeof(buf)) == 0) {
  //        const hostent *hosten = get_hostbyname(buf);
  //        if (hosten != nullptr) {
  //          ip_buffer = std::make_unique<char *>(inet_ntoa(
  //              *(reinterpret_cast<in_addr *>(*(hosten->h_addr_list)))));
  //        }
  //      }
  //#ifdef _WIN32
  //    }
  //    WSACleanup();
  //#endif
  //    return ip_buffer;
  //  }
};

#ifdef WIN32
typedef std::wstring string__;
#else
typedef std::string string__;
#endif

class os_utilities {
public:
  static void __cdecl sleep(unsigned long ms) {
#ifdef WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
  }

  string__ static current_module_path() {
    string__ service_path;
#ifdef _WIN32
    WCHAR szPath[MAX_PATH];
    if (!GetModuleFileName(NULL, szPath, MAX_PATH)) {
      wprintf(L"Cannot get service file name, error %u\n", GetLastError());
    }
    const utility::string_t module_path(szPath);
    service_path =
        module_path.substr(0, module_path.find_last_of(_XPLATSTR("\\")))
            .append(_XPLATSTR("\\"));
#else
    service_path = string__(getcwd(NULL, 0));
    service_path.append(_XPLATSTR("/"));
#endif
    return service_path;
  }

#ifndef WIN32
  static bool terminate_process(const std::string &procName) {
    bool result = false;
    int pid = getProcIdByName(procName);
    if (pid > 0) {
      int ret = kill(pid, SIGTERM); //??? SIGINT
      if (ret == 0)
        result = true;
    }
    return result;
  }

  static int getProcIdByName(const std::string &procName) {
    int pid = -1;
    // Open the /proc directory
    DIR *dp = opendir("/proc");
    if (dp != NULL) {
      // Enumerate all entries in directory until process found
      struct dirent *dirp;
      while (pid < 0 && (dirp = readdir(dp))) {
        // Skip non-numeric entries
        int id = atoi(dirp->d_name);
        if (id > 0) {
          // Read contents of virtual /proc/{pid}/cmdline file
          std::string cmdPath =
              std::string("/proc/") + dirp->d_name + "/cmdline";
          std::ifstream cmdFile(cmdPath.c_str());
          std::string cmdLine;
          std::getline(cmdFile, cmdLine);
          if (!cmdLine.empty()) {
            // Keep first cmdline item which contains the program path
            size_t pos = cmdLine.find('\0');
            if (pos != std::string::npos)
              cmdLine = cmdLine.substr(0, pos);
            // Keep program name only, removing the path
            pos = cmdLine.rfind('/');
            if (pos != std::string::npos)
              cmdLine = cmdLine.substr(pos + 1);
            // Compare against requested process name
            if (procName == cmdLine)
              pid = id;
          }
        }
      }
    }
    closedir(dp);
    return pid;
  }
#endif
};
} // namespace utils
#endif
