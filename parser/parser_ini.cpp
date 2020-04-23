#include "parser_ini.h"
#include "ace/OS_NS_stdlib.h"
#include "constants.h"
#include "tools.h"
#include <cpprest/asyncrt_utils.h>
#include <tracer.h>

namespace itvpnagent {

#ifdef _WIN32
const string_t LIC_INI_FILE = _XPLATSTR("itvpnagent.ini");
#else
const string_t LIC_INI_FILE = _XPLATSTR("lic_check_l.ini");
#endif

string_t Parser::get_service_path() { return service_path_; }

string_t Parser::make_service_path() {
  const string_t service_path = System::current_module_path();
  DEBUG_LOG((TM("Current service path: ") + service_path).c_str());
  return service_path;
}

string_t Parser::make_config_file_path() {
  string_t path_;
#ifdef _WIN32
  if (!service_path_.empty())
    path_ = this->service_path_ + this->file_name_;
  else
    path_ = this->file_name_;
#else
  // path_ = this->config_file_path_;
  path_ = this->service_path_ + this->file_name_;
  DEBUG_LOG((TM("Current config path: ") + path_).c_str());
#endif
  return path_;
}

void Parser::create_root(const string_t &file_name) {
  ifstream_t file(file_name, std::ios::in);
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

Parser::Parser(const string_t &config_file_path)
    : file_name_(LIC_INI_FILE), config_file_path_(config_file_path) {}

string_t Parser::get_value(const utility::string_t &key) const {
  string_t value;
  if (!root_.empty()) {
    value = root_.get<string_t>(key);
    if (value.empty())
      throw std::runtime_error(conversions::to_utf8string(key) +
                               " is empty. Look at the " +
                               conversions::to_utf8string(this->file_name_));
    else
      DEBUG_LOG((key + TM(" - ") + value).c_str());
  }
  return value;
}

utility::string_t Parser::get_config_file_path() {
  return this->config_file_path_;
}

string_t Parser::get_config_file_name() { return this->file_name_; }

string_t Parser::get_log_file_path() { return this->log_file_path_; }

int Parser::make_paths() {
  this->service_path_ =
      Files::split_file_path(this->get_config_file_path().c_str());
  this->file_name_ =
      Files::split_file_name(this->get_config_file_path().c_str());
  this->log_file_path_ = this->get_service_path() + _XPLATSTR("logs");
  LOGGER::instance2nd(this->get_log_file_path());
  DEBUG_LOG((TM("Current logs path: ") + this->get_log_file_path()).c_str());
  DEBUG_LOG((TM("Current service path: ") + this->get_service_path()).c_str());
  return this->init();
}

int Parser::init() {
  try {
    Options options;
    create_root(make_config_file_path());
    options.service_path = get_service_path();
    options.unp = get_value(lic::config_keys::LICENSE_UNP);
    options.prod = get_value(lic::config_keys::LICENSE_PROD);
    options.agentId = get_value(lic::config_keys::LICENSE_AGENT_ID);
    options.lic_app = get_value(lic::config_keys::FILES_LIC);
    options.lic_file = get_value(lic::config_keys::FILES_LIC_FILE_NAME);
    options.make_uid_cmd = get_value(lic::config_keys::LICENSE_MAKE_UID_CMD);
    options.uid_file_name = get_value(lic::config_keys::FILES_UID_FILE_NAME);
    options.license_manager_uri = get_value(lic::config_keys::LICENSE_SRV_URI);

    const std::string file_name_utf8_ =
        utility::conversions::to_utf8string(this->file_name_);

    const string_t lic_update_day =
        get_value(lic::config_keys::CONFIG_DAY_LICENSE_UPDATE);
    options.day_license_update = ACE_OS::atol(lic_update_day.c_str());
    if (options.day_license_update <= 0 || options.day_license_update > 31)
      throw std::runtime_error(utility::conversions::to_utf8string(
                                   lic::config_keys::CONFIG_DAY_LICENSE_UPDATE +
                                   _XPLATSTR(" isn't correct: ") +
                                   lic_update_day +
                                   _XPLATSTR(". Look at the ")) +
                               file_name_utf8_);

    options.day_license_check = ACE_OS::atol(
        get_value(lic::config_keys::CONFIG_DAY_LICENSE_CHECK).c_str());
    if (options.day_license_check <= 0 || options.day_license_check > 31)
      throw std::runtime_error(utility::conversions::to_utf8string(
                                   lic::config_keys::CONFIG_DAY_LICENSE_CHECK) +
                               " isn't correct. Look at the " + file_name_utf8_);

    options.next_try_get_license_mins = ACE_OS::atoi(
        get_value(lic::config_keys::CONFIG_NEXT_TRY_GET_LIC).c_str());
    if (options.next_try_get_license_mins <= 0 ||
        options.next_try_get_license_mins > 60)
      throw std::runtime_error(utility::conversions::to_utf8string(
                                   lic::config_keys::CONFIG_NEXT_TRY_GET_LIC) +
                               " more than 60 secs. Look at the " +
                               file_name_utf8_);

    options.next_day_waiting_hours = ACE_OS::atoi(
        get_value(lic::config_keys::CONFIG_NEXT_DAY_WAIT_GET).c_str());
    if (options.next_day_waiting_hours <= 0 ||
        options.next_day_waiting_hours > 24)
      throw std::runtime_error(utility::conversions::to_utf8string(
                                   lic::config_keys::CONFIG_NEXT_DAY_WAIT_GET) +
                               " more than 60 secs. Look at the " +
                               file_name_utf8_);

    options.kill_file_name = get_value(lic::config_keys::FILES_KILL_FILE_NAME);
    options.openvpn_file_path = get_value(lic::config_keys::FILES_OPENVPN_PATH);

    options.lic_file_name = Files::split_file_name(options.lic_file.c_str());

    options.log_files_path = get_log_file_path();
    this->set_options(options);
  } catch (const std::exception &ex) {
    ERROR_LOG((TM("Failed to initialize values of config: ") +
               conversions::to_string_t(ex.what()))
                  .c_str());
    return -1;
  }
  return 0;
}
} // namespace itvpnagent
