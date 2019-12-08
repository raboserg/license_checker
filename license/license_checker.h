// license_checker.h

#ifndef LICENSE_CHECKER_H
#define LICENSE_CHECKER_H

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

using namespace std;
namespace pt = boost::property_tree;

class LisenceChecker {
  const string file_name_;
  pt::ptree root_;

public:
  LisenceChecker(const string &file_name);
  string get_value(const string &key) const;
};

#endif
