#pragma once
#include <string>

//namespace a::b { int i; }

namespace itvpnagent {
namespace System {

#ifdef WIN32
typedef std::wstring string__;
#else
typedef std::string string__;
#endif

static void __cdecl sleep(unsigned long ms);
string__ current_module_path();
bool terminate_process(const string__ &procName);
#ifndef _WIN32
int getProcIdByName(const std::string &procName);
#endif // !_WIN32

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

} // namespace System
} // namespace itvpnagent
