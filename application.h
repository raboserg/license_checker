// applicaton.h
#ifndef APPLICATION_H
#define APPLICATION_H

#if defined(_MSC_VER)
#pragma once
#endif

#include "client_license.h"
#include "license_checker.h"
#include "platform.hpp"
#include "server.hpp"
#include <boost/program_options.hpp>
#include <iostream>

namespace itvpn {

struct LogInfo {
  LogInfo() {}
  LogInfo(std::string str) : text(std::move(str)) {}
  std::string text; // log output (usually but not always one line)
};

} // namespace itvpn

#endif
