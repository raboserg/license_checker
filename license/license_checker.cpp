// license_checker.cpp

#include "license_checker.h"
#include <iostream>

LicenseChecker::LicenseChecker() {}

int LicenseChecker::check_license(const std::string &command) {
  int result = 0;
  bp::ipstream is;
  bp::child c(command, bp::std_out > is);
  c.wait();
  std::string line;
  std::getline(is, line);
  if (!line.empty()) {
    if (line.compare("ERROR: Code: ")) {
      result = -1;
    } else if (line.compare("SUCCESS")) {
      result = 0;
    }
    std::cout << line << endl;
  } else {
    result = -1;
  }
  return result;
}
