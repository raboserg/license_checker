#include "license_checker.h"

LicenseChecker::LicenseChecker() {}

utility::string_t LicenseChecker::run_proc(const utility::string_t &command) {
  bp_is is;
  bp::child process(command, bp::std_out > is);
  process.wait();
  utility::string_t line;
  std::getline(is, line);
  return line;
}

bool LicenseChecker::check_license(const utility::string_t &command) {
  bool result = false;
	utility::string_t line = run_proc(command);
  if (line.empty())
    throw "lic of output is empty.";
  else {
    const utility::string_t code = line.substr(0, line.find_first_of(U(":")));
    if (code == U("ERROR")) {
      result = false;
    } else if (code == U("SUCCESS")) {
      result = true;
    }
  }
  return result;
}

utility::string_t
LicenseChecker::make_file_license(const utility::string_t &command) {
  utility::string_t line = run_proc(command);
  if (line.empty())
    throw "does not make file by lic";
  return line;
}
