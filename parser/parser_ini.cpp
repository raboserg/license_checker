#include "parser_ini.h"

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

void Parser::create_root(const utility::string_t &file_name) {
  utility::ifstream_t file(file_name, std::ios::in);
  //  boost::iostreams::filtering_ostream out;
  //  out.push(skip_bom(file_name));
  if (!file.is_open()) {
    //?? check else not open
    l_iTrace->P7_ERROR(nullptr, TM("INI file was not opened"), 0);
    // std::cout << "INI file was not opened" << std::endl;
  } else { // skip BOM
    unsigned char buffer[8];
    buffer[0] = 255;
    while (file.good() && buffer[0] > 127)
      file.read(reinterpret_cast<utf16char *>(buffer), 1);
    std::streamoff pos = file.tellg();
    if (pos > 0)
      file.seekg(pos - 1);
    //pt::read_ini(file, root_);
    file.close();
  }
}

Parser::Parser(const utility::string_t &file_name)
    : file_name_(std::move(file_name)) {
  l_iTrace = P7_Get_Shared_Trace(TM("LICENSE_CHECKER_TRC_LOG"));
  create_root(file_name);
}

utility::string_t Parser::get_value(const utility::string_t &key) const {
  utility::string_t value;
  l_iTrace->P7_TRACE(0, key.c_str(), 0);
  if (!root_.empty()) {
		value = root_.get<utility::string_t>(key);
  }
  return value;
}

Parser::~Parser() {
  if (l_iTrace) {
    l_iTrace->Release();
    l_iTrace = nullptr;
  }
}
