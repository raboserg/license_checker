// license_checker.h
#pragma once
#ifndef LICENSE_CHECKER_H
#define LICENSE_CHECKER_H

#include "parser_ini.h"
#include <boost/process.hpp>

using namespace std;
namespace pt = boost::property_tree;
namespace bp = boost::process;

class LicenseChecker {
  const string file_name_;
  pt::ptree root_;

public:
  LicenseChecker();
  int check_license(const std::string &command);
};

#endif
