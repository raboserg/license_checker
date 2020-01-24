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

bool LicenseChecker::verify_license_file() {
  bool result = false;
  utility::string_t line = run_proc(lic::license_helper::make_verify_license_cmd());
  if (line.empty()) {
    throw std::runtime_error("lic of output is empty");
  } else {
    const utility::string_t code = line.substr(0, line.find_first_of(U(":")));
    if (code == U("ERROR")) {
      result = false;
    } else if (code == U("SUCCESS")) {
      result = true;
    } else {
      throw std::runtime_error("lic returned invalid responce");
    }
  }
  return result;
}

utility::string_t LicenseChecker::generate_machine_uid() {
	const utility::string_t line = run_proc(lic::license_helper::make_machine_uid_cmd());
  if (line.empty()) {
    throw std::runtime_error("does not make file by lic");
    // throw "does not make file by lic";
  }
  return line;
}

void LicenseChecker::save_license_to_file(const utility::string_t &command) {}
