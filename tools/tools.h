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

namespace utilities {

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

class os_utilities {
public:
  static void __cdecl sleep(unsigned long ms) {
#ifdef WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
  }
};

struct my_exception : public std::exception {
	const std::string message;
	my_exception(const std::string message) :message() {}
	const char* what() noexcept {
		return "my_exception::what()";
	}
};
// static std::unique_ptr<const wchar_t *> char_to_wchar_(const char *args) {
//  wchar_t dst[100];
//  size_t outSize;
//  mbstowcs_s(&outSize, dst, _countof(dst), args, _TRUNCATE);
//  return std::make_unique<const wchar_t *>(dst);
//}

// static std::wstring char_to_wchar(const char *args) {
//#ifdef _WIN32
//  wchar_t dst[100];
//  size_t outSize;
//  mbstowcs_s(&outSize, dst, _countof(dst), args, _TRUNCATE);
//  std::wstring fdsfds(dst);
//#else
//  std::string fdsfds(args);
//#endif // _WIN32
//  return fdsfds;
//}

} // namespace utilities
#endif
