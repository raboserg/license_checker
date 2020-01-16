#pragma once
#ifndef LICENSE_CHECKER_H
#define LICENSE_CHECKER_H

#include <P7_Trace.h>
#include <boost/process.hpp>
#include <cpprest/details/basic_types.h>
#include <parser_ini.h>
#include <tracer.h>

namespace bp = boost::process;

#ifdef _WIN32
typedef bp::wipstream bp_is;
#else
typedef bp::ipstream bp_is;
#endif

class LicenseChecker {
  utility::string_t run_proc(const utility::string_t &command);

public:
  LicenseChecker();
  bool check_license(const utility::string_t &command);
  utility::string_t make_file_license(const utility::string_t &command);
};

#endif
