// license_checker.h
#pragma once
#ifndef LICENSE_CHECKER_H
#define LICENSE_CHECKER_H

#include "parser_ini.h"
#include <boost/process.hpp>

using namespace std;
namespace bp = boost::process;

class LicenseChecker {
public:
  LicenseChecker();
  bool check_license(const std::string &command);
};

#endif
