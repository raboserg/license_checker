// applicaton.h
#ifndef APPLICATION_H
#define APPLICATION_H

#if defined(_MSC_VER)
#pragma once
#endif

#include "platform.hpp"
#include "client_license.h"
#include "license_checker.h"
#include <iostream>
#include <boost/program_options.hpp>

namespace itvpn {

struct LogInfo {
  LogInfo() {}
  LogInfo(std::string str) : text(std::move(str)) {}
  std::string text; // log output (usually but not always one line)
};

} // namespace itvpn

#endif
