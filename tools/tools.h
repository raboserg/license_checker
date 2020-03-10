#pragma once
#ifndef TOOLS_H
#define TOOLS_H

#ifdef _WIN32
#include <Windows.h>
#else
#include "ace/Log_Msg.h"
#include "ace/OS_NS_unistd.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <cpprest/details/basic_types.h>
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

class os_utilities {
public:
  static void __cdecl sleep(unsigned long ms) {
#ifdef WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
  }

  utility::string_t static current_module_path() {
    utility::string_t service_path;
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
    char const *buf = getcwd(NULL, 0);
    utility::char_t original_pathname[MAXPATHLEN + 1];
    if (ACE_OS::getcwd(original_pathname, MAXPATHLEN + 1) == 0)
      ACE_ERROR((LM_ERROR, "%p\n%a", "", 1));
    const utility::string_t fdfds(original_pathname);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("PATH: %s \n"),
               utility::string_t(original_pathname).c_str()));
    service_path = utility::string_t(original_pathname);
    service_path.append(_XPLATSTR("/"));
#endif
    return service_path;
  }
};
} // namespace utils
#endif
