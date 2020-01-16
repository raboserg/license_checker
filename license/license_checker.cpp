#include "license_checker.h"

LicenseChecker::LicenseChecker() {}

utility::string_t LicenseChecker::run_proc(const utility::string_t &command) {
  bp::ipstream is;
  bp::child process(command, bp::std_out > is);
  process.wait();
  std::string line;
  std::getline(is, line);
  return utility::conversions::to_string_t(line);
}

bool LicenseChecker::check_license(const utility::string_t &command) {
  bool result = false;
  utility::string_t line = run_proc(command);
  if (line.empty()) {
    TRACE(0, TM("lic of output is empty"));
    throw "lic of output is empty";
  } else {
    const utility::string_t code = line.substr(0, line.find_first_of(U(":")));
    if (code == U("ERROR")) {
      result = true;
    } else if (code == U("SUCCESS")) {
      TRACE(0, TM("License is SUCCESS"));
    }
  }
  return result;
}

utility::string_t
LicenseChecker::make_file_license(const utility::string_t &command) {
  TRACE(0, command.c_str());
  utility::string_t line = run_proc(command);
  if (line.empty()) {
    ERRORS(0, TM("does not make file by lic"));
    throw "does not make file by lic";
  }
  return line;
}
