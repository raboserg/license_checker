#include "license_checker.h"
#include "parser_ini.h"
#include <constants.h>
#include <cpprest/asyncrt_utils.h>
#include <tracer.h>

#include "ace/Date_Time.h"
#include "ace/Log_Msg.h"
#include "ace/OS_NS_stdlib.h"

LicenseChecker::LicenseChecker() {}

utility::string_t LicenseChecker::run_proc(const utility::string_t &command) {
  bp::ipstream is;
  bp::child process(command, bp::std_out > is);
  process.wait();
  std::string line;
  std::getline(is, line);
  return utility::conversions::to_string_t(line);
}

bool LicenseChecker::verify_license_file() {
  bool result = false;
  utility::string_t line = run_proc(make_verify_license_cmd());
  if (line.empty()) {
    throw std::runtime_error("lic of output is empty");
  } else {
    const utility::string_t code =
        line.substr(0, line.find_first_of(_XPLATSTR(":")));
    if (code == _XPLATSTR("ERROR")) {
      result = false;
    } else if (code == _XPLATSTR("SUCCESS")) {
      result = true;
    } else {
      throw std::runtime_error("lic returned invalid responce");
    }
  }
  return result;
}

utility::string_t LicenseChecker::generate_machine_uid() {
  utility::string_t line = run_proc(make_machine_uid_cmd());
  if (line.empty()) {
    throw std::runtime_error("does not make file by lic");
  }
  // delete carriage return
  const size_t len = line.length();
  if (len && (line.c_str()[len - 1] == 0x0D))
    line.erase(len - 1);

  return line;
}

void LicenseChecker::save_license_to_file(const utility::string_t &license) {
  // save license to file
  const utility::string_t lic_file_name =
      PARSER::instance()->get_value(lic::config_keys::FILES_LIC_FILE_NAME);
  utility::ofstream_t file(lic_file_name, std::ios::out);
  if (file.is_open()) {
    file.write(reinterpret_cast<const utility::char_t *>(license.c_str()),
               sizeof(utility::char_t) * license.size());
    file.close();
  } else {
    //?????
  }
}

utility::string_t LicenseChecker::make_verify_license_cmd() {
  //[LICENSE] check_lic_cmd = -v --lic
  //[FILES] lic_file_name = license_item.lic
  //[FILES] lic=D:\work\itvpn_setup\itvpn\bin\x64\lic.exe
  utility::string_t license_process_path =
      PARSER::instance()->get_value(lic::config_keys::FILES_LIC);
  const utility::string_t check_lic_cmd =
      PARSER::instance()->get_value(lic::config_keys::LICENSE_CHECK_LIC_CMD);
  const utility::string_t license_file_name =
      PARSER::instance()->get_value(lic::config_keys::FILES_LIC_FILE_NAME);

  if (license_process_path.empty() && check_lic_cmd.empty() &&
      license_file_name.empty()) {
    throw std::runtime_error("Not create command varify license");
  }

  license_process_path.append(_XPLATSTR(" "))
      .append(check_lic_cmd)
      .append(_XPLATSTR(" "))
      .append(license_file_name);

  INFO_LOG(license_process_path.c_str());

  return license_process_path;
}

utility::string_t LicenseChecker::make_machine_uid_cmd() {
  //[LICENSE] prod = 2
  //[LICENSE] make_lic_cmd = -g -s --prod
  //[FILES]	lic = D:\work\itvpn_setup\itvpn\bin\x64\lic.exe
  utility::string_t license_process_path;
  license_process_path =
      PARSER::instance()->get_value(lic::config_keys::FILES_LIC);
  const utility::string_t make_lic_cmd =
      PARSER::instance()->get_value(lic::config_keys::LICENSE_MAKE_LIC_CMD);
  const utility::string_t license_prod =
      PARSER::instance()->get_value(lic::config_keys::LICENSE_PROD);
  const utility::string_t license_uid =
      PARSER::instance()->get_value(lic::config_keys::LICENSE_UID);

  if (license_process_path.empty() && make_lic_cmd.empty() &&
      license_prod.empty()) {
    throw std::runtime_error("Not create command generate machine uid");
  }

  license_process_path.append(_XPLATSTR(" "))
      .append(make_lic_cmd)
      .append(_XPLATSTR(" "))
      .append(license_prod);

  INFO_LOG(license_process_path.c_str());

  return license_process_path;
}

bool LicenseChecker::check_day_updete() {
  ACE_Date_Time date_time;
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Day of today %d\n"), date_time.day()));
  const utility::string_t license_update_day =
      PARSER::instance()->get_value(lic::config_keys::LICENSE_DAY_FOR_UPDATE);
  if (license_update_day.empty())
    ACE_ERROR_RETURN(
        (LM_ERROR, ACE_TEXT("%T (%t):\tGet LICENSE_DAY_FOR_UPDATE failed\n")),
        false);
	//THROW EXEPTION - ABORT SERVICE
  const long day = ACE_OS::atol(license_update_day.c_str());
  if (date_time.day() >= day)
    return true;
  return false;
}
