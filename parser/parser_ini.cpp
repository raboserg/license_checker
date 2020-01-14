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

void Parser::create_root(const std::string &file_name) {
  std::ifstream file(file_name, std::ios::in);
  //  boost::iostreams::filtering_ostream out;
  //  out.push(skip_bom(file_name));
  if (!file.is_open()) {
    //?? check else not open
    l_iTrace->P7_ERROR(nullptr, TM("INI file was not opened"), 0);
    // std::cout << "INI file was not opened" << std::endl;
  } else {
    // skip BOM
    unsigned char buffer[8];
    buffer[0] = 255;
    while (file.good() && buffer[0] > 127)
      file.read(reinterpret_cast<char *>(buffer), 1);
    std::streamoff pos = file.tellg();
    if (pos > 0)
      file.seekg(pos - 1);
    pt::read_ini(file, root_);
    file.close();
  }
}

Parser::Parser(const std::string &file_name)
    : file_name_(std::move(file_name)) {
  l_iTrace = P7_Get_Shared_Trace(TM("LICENSE_CHECKER_TRC_LOG"));
  create_root(file_name);
}

std::string Parser::get_value(const std::string &key) const {
  std::string value;

  wchar_t WBuf[100];
  mbstowcs(WBuf, key.c_str(), 99);
  // license_checker::char_to_wchar(key.c_str());
  // l_iTrace->P7_TRACE(0, key.c_str(), 0);
  Tracer::trace(key.c_str());

  if (!root_.empty())
    value = root_.get<std::string>(key);
  return value;
}

Parser::~Parser() {
  if (l_iTrace) {
    l_iTrace->Release();
    l_iTrace = nullptr;
  }
}
