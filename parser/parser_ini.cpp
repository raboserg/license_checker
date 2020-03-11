#include "parser_ini.h"
#include "constants.h"
#include "tools.h"
#include <cpprest/asyncrt_utils.h>
#include <tracer.h>

#ifdef _WIN32
const utility::string_t LIC_INI_FILE = U("itvpnagent.ini");
#else
const utility::string_t LIC_INI_FILE = U("lic_check_l.ini");
#endif

utility::string_t Parser::get_service_path() { return service_path_; }

utility::string_t Parser::make_service_path() {
  const utility::string_t service_path =
      utils::os_utilities::current_module_path();
  DEBUG_LOG((TM("Current service path: ") + service_path).c_str());
  return service_path;
}

utility::string_t Parser::get_config_path() {
  utility::string_t path_;
  if (!service_path_.empty())
    path_ = this->service_path_ + this->file_name_;
  else
    path_ = this->file_name_;
  DEBUG_LOG((TM("Current config path: ") + path_).c_str());
  return path_;
}

void Parser::create_root(const utility::string_t &file_name) {
  utility::ifstream_t file(file_name, std::ios::in);
  if (!file.is_open()) {
    const std::string error_msg(
        std::string("can't open ini file - ")
            .append(utility::conversions::to_utf8string(file_name)));
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
    : file_name_(LIC_INI_FILE), service_path_(make_service_path()) {}

Parser::Parser(const utility::string_t &file_name)
    : file_name_(file_name), service_path_(make_service_path()) {}

utility::string_t Parser::get_value(const utility::string_t &key) const {
  utility::string_t value;
  if (!root_.empty()) {
    value = root_.get<utility::string_t>(key);
  }
  DEBUG_LOG((key + TM(" - ") + value).c_str());
  return value;
}

int Parser::init() {
  try {
    create_root(get_config_path());
    Options options;
    options.unp = get_value(lic::config_keys::LICENSE_UNP);
    options.prod = get_value(lic::config_keys::LICENSE_PROD);
    options.agentId = get_value(lic::config_keys::LICENSE_AGENT_ID);
    options.lic_app = get_value(lic::config_keys::FILES_LIC);
    options.lic_file = get_value(lic::config_keys::FILES_LIC_FILE_NAME);
    options.make_uid_cmd = get_value(lic::config_keys::LICENSE_MAKE_UID_CMD);
    options.uid_file_name = get_value(lic::config_keys::FILES_UID_FILE_NAME);
    options.license_manager_uri = get_value(lic::config_keys::LICENSE_SRV_URI);
    options.day_license_update =
        get_value(lic::config_keys::CONFIG_DAY_LICENSE_UPDATE);
    options.day_license_check =
        get_value(lic::config_keys::CONFIG_DAY_LICENSE_CHECK);
    options.next_try_get_license_mins =
        get_value(lic::config_keys::CONFIG_NEXT_TRY_GET_LIC);
    options.kill_file_name = get_value(lic::config_keys::FILES_KILL_FILE_NAME);
    options.next_day_waiting_hours =
        get_value(lic::config_keys::CONFIG_NEXT_DAY_WAIT_GET);

    // int pos = options.lic_app.find_last_of(_XPLATSTR("\\"));
    //   if (pos != string_t::npos)
    //     options.lic_files_path = options.lic_app.substr(0, pos);
    //   else
    //     options.lic_files_path = _XPLATSTR(".");

    // pos = options.lic_file.find_last_of(_XPLATSTR("\\"));
    // if (pos != string_t::npos) {
    //	options.lic_file_name = options.lic_file.substr(0, pos);
    //
    //	options.lic_file_name =
    //		options.lic_file.substr(options.lic_file.find_last_of('\\') +
    //1);
    //}
    // else
    //	options.lic_file_name = _XPLATSTR(".");

    options.lic_file_name =
        options.lic_file.substr(options.lic_file.find_last_of('\\') + 1);

    options.log_files_path = get_service_path() + _XPLATSTR("logs");
    this->set_options(options);
  } catch (const std::exception &ex) {
    ERROR_LOG((TM("Failed to initialize options values: ") +
               conversions::to_string_t(ex.what()))
                  .c_str());
    return -1;
  }
  return 0;
}
