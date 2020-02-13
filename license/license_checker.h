#pragma once
#ifndef LICENSE_CHECKER_H
#define LICENSE_CHECKER_H

#define _TURN_OFF_PLATFORM_STRING
#include "ace/Date_Time.h"
#include "client_license.h"
#include <boost/process.hpp>
#include <cpprest/details/basic_types.h>

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
  bool check_update_day();
  bool check_license_day();
  bool verify_license_file();
  utility::string_t generate_machine_uid();
  void save_license_to_file(const utility::string_t &command);
  ACE_Date_Time extract_license_date(const utility::string_t &lic);
  std::shared_ptr<LicenseExtractor>
  make_license_extractor(const int64_t &attemp0t);
  bool is_license_file(const utility::string_t &file_name);

private:
  utility::string_t make_verify_license_cmd();
  utility::string_t make_machine_uid_cmd();
};

#endif
