// applicaton.h
#ifndef APPLICATION_H
#define APPLICATION_H

#if defined(_MSC_VER)
#pragma once
#endif

#include "client_license.h"
#include "license_checker.h"
#include "platform.hpp"
//#include "server.hpp"
//#include <boost/program_options.hpp>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/uri.h>
#include <iostream>
#include <thread>

#ifdef _WIN32
#include <winsock2.h>
#include <notificator.h>
#include "service.h"
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif
#include <sys/types.h>

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

#endif
