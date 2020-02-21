#include "parser_ini.h"
#include <cpprest/asyncrt_utils.h>
#include <tracer.h>

#ifdef _WIN32
const utility::string_t LIC_INI_FILE = U("lic_check_w.ini");
#else
const utility::string_t LIC_INI_FILE = U("lic_check_l.ini");
#endif

utility::string_t Parser::get_service_path() {
  return service_path_;
}

utility::string_t Parser::make_service_path() {
  utility::string_t service_path;
#ifdef _WIN32
  WCHAR szPath[MAX_PATH];
  if (!GetModuleFileName(NULL, szPath, MAX_PATH)) {
    wprintf(L"Cannot get service file name, error %u\n", GetLastError());
  }
  const utility::string_t module_path(szPath);
	service_path = module_path.substr(0, module_path.find_last_of(_XPLATSTR("\\")))
                    .append(_XPLATSTR("\\"));
#endif
  INFO_LOG((TM("Current service path: ") + service_path).c_str());
  return service_path;
}

utility::string_t Parser::get_config_path() {
  utility::string_t path_;
  if (service_path_.empty())
    path_ = this->service_path_.append(this->file_name_);
  else
    path_ = this->file_name_;
  return path_;
}

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

Parser::Parser()
    : file_name_(LIC_INI_FILE), service_path_(make_service_path()) {
  create_root(get_config_path());
}

Parser::Parser(const utility::string_t &file_name)
    : file_name_(file_name), service_path_(make_service_path()) {
  create_root(get_config_path());
}

utility::string_t Parser::get_value(const utility::string_t &key) const {
  utility::string_t value;
  if (!root_.empty()) {
    value = root_.get<utility::string_t>(key);
  }
  DEBUG_LOG((key + TM(" - ") + value).c_str());
  return value;
}

Parser::~Parser() {}
