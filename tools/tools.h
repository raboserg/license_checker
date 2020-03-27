#pragma once
#define WIN32_LEAN_AND_MEAN
#ifdef _WIN32
#include <Windows.h>
#endif
#include <string>

namespace itvpnagent {

#ifdef WIN32
using string__ = std::wstring;
using char__ = wchar_t;
#else
using std::string string__;
using char__ = char;
#endif

namespace System {

void sleep(const unsigned long ms);
string__ current_module_path();
bool terminate_process(const string__ &procName);

#ifndef _WIN32
int getProcIdByName(const std::string &procName);
#endif // !_WIN32

} // namespace System

namespace Files {
string__ get_file_name_from_path(const char__ *buffer);
string__ get_path_without_file_name(const char__ *buffer);
}

namespace Net {

int send_message(const string__ message);

// struct ip_helper {
//
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
//};

} // namespace Net
} // namespace itvpnagent
