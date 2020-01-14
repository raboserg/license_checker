#include "license_checker.h"
#include "parser_ini.h"

LicenseChecker::LicenseChecker() {}

bool LicenseChecker::check_license(const std::string &command) {
  bool result = false;
  bp::ipstream is;
  bp::child process(command, bp::std_out > is);
  process.wait();
  std::string line;
  std::getline(is, line);
  if (line.empty())
    throw "lic of output is empty.";
  else {
    const std::string code = line.substr(0, line.find_first_of(":"));
    if (code == "ERROR") {
      result = false;
    } else if (code == "SUCCESS") {
      result = true;
    }
  }
  return result;
}
