#include "parser_ini.h"

#ifdef _WIN32
const utility::string_t LIC_INI_FILE = U("lic_check_w.ini");
#else
const utility::string_t LIC_INI_FILE = U("lic_check_l.ini");
#endif

void Parser::create_root(const utility::string_t &file_name) {
  utility::ifstream_t file(file_name, std::ios::in);
  if (!file.is_open()) {
    const std::string error_msg(
        std::string("can't open ini file - ")
            .append(utility::conversions::to_utf8string(file_name)));
    ERROR_LOG(utility::conversions::to_string_t(error_msg).c_str());
    throw std::runtime_error(error_msg); // exeption in constructor ???
  } else {                               // skip BOM
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

Parser::Parser() : file_name_(std::move(LIC_INI_FILE)) {
#ifdef _WIN32
  WCHAR szPath[MAX_PATH];
  if (!GetModuleFileName(NULL, szPath, MAX_PATH)) {
    wprintf(L"Cannot install service, error %u\n", GetLastError());
  }
#endif
  create_root(file_name_);
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
