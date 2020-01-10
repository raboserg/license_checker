// license_checker.cpp

#include <iostream>
#include "license_checker.h"

LicenseChecker::LicenseChecker() {}

int LicenseChecker::check_license(const std::string &command) {
  int result = 0;
  bp::ipstream is;
  bp::child c(command, bp::std_out > is);
  c.wait();
  std::string line;
  std::getline(is, line);
  if (!line.empty()) {

    "ERROR: Code: ";
    "SUCCESS";
    std::cout << line << endl;
  } else {
    result = -1;
  }
  return result;
}
