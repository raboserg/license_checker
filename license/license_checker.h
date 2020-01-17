#pragma once
#ifndef LICENSE_CHECKER_H
#define LICENSE_CHECKER_H

#include <boost/process.hpp>
#include <cpprest/details/basic_types.h>
#include <cpprest/asyncrt_utils.h>
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
  bool verify_license_file(const utility::string_t &command);
  utility::string_t generate_machine_uid(const utility::string_t &command);
	void save_license_to_file(const utility::string_t &command);
};

#endif
