#include "parser_ini.h"

void Parser::create_root(const utility::string_t &file_name) {
  utility::ifstream_t file(file_name, std::ios::in);
  if (!file.is_open()) {
    //?? check else not open
    LOGGER::getInstance()->debug(TM("can't open ini file"), __FILE__, __LINE__,
                                 __FUNCTION__);
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
  LOGGER::getInstance()->debug(key.c_str(), __FILE__, __LINE__, __FUNCTION__);
  if (!root_.empty()) {
    value = root_.get<utility::string_t>(key);
  }
  return value;
}

Parser::~Parser() {
  LOGGER::getInstance()->debug(TM("Parser::~Parser()"), __FILE__, __LINE__,
                               __FUNCTION__);
}
