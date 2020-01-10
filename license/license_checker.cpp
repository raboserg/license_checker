﻿// license_checker.cpp

#include "license_checker.h"
//#include <boost/iostreams/filtering_stream.hpp>

// struct skip_bom {
//  pt::ptree get(const string &file_name) {
//    pt::ptree root_;
//    std::ifstream file(file_name, std::ios::in);
//    if (file.is_open()) {
//      // skip BOM
//      unsigned char buffer[8];
//      buffer[0] = 255;
//      while (file.good() && buffer[0] > 127)
//        file.read((char *)buffer, 1);
//      int pos = file.tellg();
//      if (pos > 0)
//        file.seekg(pos - 1);
//      pt::read_ini(file, root_);
//      file.close();
//    }
//    return root_;
//  }
//};

LicenseChecker::LicenseChecker(const string &file_name)
    : file_name_(std::move(file_name)) {
  std::ifstream file(file_name, std::ios::in);
  //  boost::iostreams::filtering_ostream out;
  //  out.push(skip_bom(file_name));
  if (file.is_open()) {
    // skip BOM
    unsigned char buffer[8];
    buffer[0] = 255;
    while (file.good() && buffer[0] > 127)
      file.read((char *)buffer, 1);
    std::streamoff pos = file.tellg();
    if (pos > 0)
      file.seekg(pos - 1);
    pt::read_ini(file, root_);
    file.close();
  }
}

string LicenseChecker::get_value(const string &key) const {
  string value;
  if (!root_.empty())
    value = root_.get<string>(key);
  return value;
}

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
