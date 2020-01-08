﻿// license_checker.h
#pragma once
#ifndef LICENSE_CHECKER_H
#define LICENSE_CHECKER_H

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
