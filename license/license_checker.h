// license_checker.h
#pragma once
#ifndef LICENSE_CHECKER_H
#define LICENSE_CHECKER_H

#include <vector>
#include <iostream>
#include "parser_ini.h"
#include <boost/process.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

using namespace std;
namespace pt = boost::property_tree;
namespace bp = boost::process;

class LicenseChecker {
  const string file_name_;
  pt::ptree root_;

public:
  LicenseChecker(const string &file_name);
  string get_value(const string &key) const;
  int check_license(const std::string &command);
};

#endif
