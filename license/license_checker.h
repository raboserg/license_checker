#pragma once
#ifndef LICENSE_CHECKER_H
#define LICENSE_CHECKER_H

#include <parser_ini.h>
#include <boost/process.hpp>
#include <cpprest/asyncrt_utils.h>
#include <cpprest/details/basic_types.h>
#include <license_helper.h>
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
  bool verify_license_file();
  utility::string_t generate_machine_uid();
  void save_license_to_file(const utility::string_t &command);
};

#endif
