#pragma once
#ifndef LICENSE_CHECKER_H
#define LICENSE_CHECKER_H

#define _TURN_OFF_PLATFORM_STRING
#include <cpprest/details/basic_types.h>
#include <boost/process.hpp>

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
	bool check_day_updete();
	bool verify_license_file();
  utility::string_t generate_machine_uid();
  void save_license_to_file(const utility::string_t &command);
private:	
	utility::string_t make_verify_license_cmd();
	utility::string_t make_machine_uid_cmd();
};

#endif
