#include "parser_ini.h"

void Parser::create_root(const utility::string_t &file_name) {
  utility::ifstream_t file(file_name, std::ios::in);
  if (!file.is_open()) {
    //?? check else not open
		//LOGGER::instance()->debug(file_name.c_str(), __FILE__, __LINE__,			__FUNCTION__);
		DEBUGS(0, file_name.c_str());
    //LOGGER::instance()->debug(TM("can't open ini file"), __FILE__, __LINE__,                                 __FUNCTION__);
		DEBUGS(0, TM("can't open ini file"));
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
  //LOGGER::instance()->debug(key.c_str(), __FILE__, __LINE__, __FUNCTION__);
	DEBUGS(0, key.c_str());
  if (!root_.empty()) {
    value = root_.get<utility::string_t>(key);
  }
  return value;
}

Parser::~Parser() {
	DEBUGS(0, TM("Parser::~Parser()"));
}
