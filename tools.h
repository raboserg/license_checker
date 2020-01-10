#pragma once

#include <string>
#include <memory>
#include <Windows.h>

namespace itvpn {

struct LogInfo {
  LogInfo() {}
  LogInfo(std::string str) : text(std::move(str)) {}
  std::string text; // log output (usually but not always one line)
};

struct ip_helper {

  static int get_hostname(char *hostbuffer, int size) {
    return gethostname(hostbuffer, size);
  }

  static hostent *get_hostbyname(char *hostbuffer) {
    struct hostent *host_entry = nullptr;
    host_entry = gethostbyname(hostbuffer);
    return host_entry;
  }

  static std::unique_ptr<char *> get_ip() {
    char buf[128];
    std::unique_ptr<char *> ip_buffer = nullptr;
#ifdef _WIN32
    WSAData wsaData;
    const int WSVer = MAKEWORD(2, 2);
    if (WSAStartup(WSVer, &wsaData) == 0) {
#endif
      if (get_hostname(buf, sizeof(buf)) == 0) {
        const hostent *hosten = get_hostbyname(buf);
        if (hosten != nullptr) {
          ip_buffer = std::make_unique<char *>(inet_ntoa(
              *(reinterpret_cast<in_addr *>(*(hosten->h_addr_list)))));
        }
      }
#ifdef _WIN32
    }
    WSACleanup();
#endif
    return ip_buffer;
  }
};
} // namespace itvpn
