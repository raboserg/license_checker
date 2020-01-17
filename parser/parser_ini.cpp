#include "parser_ini.h"

void Parser::create_root(const utility::string_t &file_name) {
  utility::ifstream_t file(file_name, std::ios::in);
  if (!file.is_open()) {
    //?? check else not open
    DEBUG_LOG(utility::conversions::to_string_t(U("can't open ini file - ") + file_name).c_str());
  } else { // skip BOM
    unsigned char buffer[8];
    buffer[0] = 255;
    while (file.good() && buffer[0] > 127)
      file.read(reinterpret_cast<utility::char_t *>(buffer), 1);
    std::streamoff pos = file.tellg();
    if (pos > 0)
      file.seekg(pos - 1);
    pt::read_ini(file, root_);
    file.close();
  }
}

Parser::Parser(const utility::string_t &file_name)
    : file_name_(std::move(file_name)) {
  create_root(file_name);
}

utility::string_t Parser::get_value(const utility::string_t &key) const {
  utility::string_t value;
  DEBUG_LOG(key.c_str());
  if (!root_.empty()) {
    value = root_.get<utility::string_t>(key);
  }
  return value;
}

Parser::~Parser() { DEBUG_LOG(TM("Parser::~Parser()")); }
