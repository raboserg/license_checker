#pragma once
#ifndef LICENSE_CHECKER_H
#define LICENSE_CHECKER_H

#define _TURN_OFF_PLATFORM_STRING
#include "ace/Date_Time.h"
#include "boost/filesystem.hpp"
#include "client_license.h"
#include <boost/process.hpp>
#include <cpprest/details/basic_types.h>

namespace bp = boost::process;

using namespace std;
using namespace utility;
using namespace boost::filesystem;

#ifdef _WIN32
typedef bp::wipstream bp_is;
#else
typedef bp::ipstream bp_is;
#endif

class LicenseChecker {
  string_t run_proc(const string_t &command);

public:
  LicenseChecker();
  bool is_license_update_day();
  bool is_license_check_day();
  bool verify_license();
  string_t generate_machine_uid();
  bool save_license_to_file(string_t &license);
  ACE_Date_Time extract_license_date(const string_t &lic);
  shared_ptr<LicenseExtractor> make_license_extractor(const int64_t &attemp0t);
  bool is_license_file();
  ACE_Date_Time current_license_date();
  string_t read_license_from_file(const string_t &file_name);
  bool is_check_licenses_months(const int &next_month);

private:
  string_t make_verify_license_cmd();
  string_t make_machine_uid_cmd();
  bool find_file(const path &dir_path, const string_t &file_name,
                 path &path_found);
};

#endif
