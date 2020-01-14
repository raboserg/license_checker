#pragma once
#ifndef LICENSE_CHECKER_H
#define LICENSE_CHECKER_H

#include <P7_Trace.h>
#include <boost/process.hpp>

namespace bp = boost::process;

class LicenseChecker {
public:
  LicenseChecker();
  bool check_license(const std::string &command);
};

#endif
