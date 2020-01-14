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
#endif

#include <memory>

namespace license_checker {

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

//static wchar_t* char_to_wchar(const char* args) {
//	wchar_t dst[100];
//	size_t outSize;
//	mbstowcs_s(&outSize, dst, _countof(dst), args, _TRUNCATE);
//	return dst;
//}

} // namespace license_checker
#endif
