#include "license_checker.h"
#include "parser_ini.h"
#include <constants.h>
#include <cpprest/asyncrt_utils.h>
#include <tracer.h>

//#include "ace/Auto_Ptr.h"
//#include "ace/Codecs.h"

#include "ace/Log_Msg.h"
#include "ace/OS_NS_stdlib.h"

LicenseChecker::LicenseChecker() {}

utility::string_t LicenseChecker::run_proc(const string_t &command) {
  bp::ipstream is;
  bp::child process(command, bp::std_out > is);
  process.wait();
  std::string line;
  std::getline(is, line);
  return utility::conversions::to_string_t(line);
}

bool LicenseChecker::is_license_file(const string_t &file_name) {
  bool result = false;
  const string_t license_file_name =
      PARSER::instance()->get_value(lic::config_keys::FILES_LIC_FILE_NAME);
  ifstream_t file(license_file_name, std::ios::out);
  if (file.is_open()) {
    file.close();
    result = true;
  }
  return result;
}
bool LicenseChecker::verify_license() {
  bool result = false;
  string_t line = run_proc(make_verify_license_cmd());
  if (line.empty()) {
    throw std::runtime_error("lic of output is empty");
  } else {
    const string_t code = line.substr(0, line.find_first_of(_XPLATSTR(":")));
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

string_t LicenseChecker::generate_machine_uid() {
  string_t line = run_proc(make_machine_uid_cmd());
  if (line.empty()) {
    throw std::runtime_error("does not make file by lic");
  }
  // delete carriage return
  const size_t len = line.length();
  if (len && (line.c_str()[len - 1] == 0x0D))
    line.erase(len - 1);

  if (line.length() != lic::constants::UID_SIZE)
    throw std::runtime_error("size of UID isn't valid: " +
                             std::to_string(line.length()));
  return line;
}

void LicenseChecker::save_license_to_file(string_t &license) {

  const size_t len = license.length();
  if (len && (license.c_str()[len - 1] == 0x00))
    license.erase(len - 1);

  // save license to file
  const string_t lic_file_name =
      PARSER::instance()->get_value(lic::config_keys::FILES_LIC_FILE_NAME);
  ofstream_t file(lic_file_name, std::ios::out);
  if (file.is_open()) {
		file << license;
    file.close();
  } else {
    //?????
  }
}

string_t LicenseChecker::read_license_from_file(const string_t &file_name) {
  // read license to file
  string_t license;
  ifstream_t file(file_name, std::ios::in);
  if (file.is_open()) {
    file >> license;
    file.close();
  } else {
    //?????
  }
  return license;
}

string_t LicenseChecker::make_verify_license_cmd() {
  //[LICENSE] check_lic_cmd = -v --lic
  //[FILES] lic_file_name = license_item.lic
  //[FILES] lic=D:\work\itvpn_setup\itvpn\bin\x64\lic.exe
  string_t license_process_path =
      PARSER::instance()->get_value(lic::config_keys::FILES_LIC);
  const string_t check_lic_cmd =
      PARSER::instance()->get_value(lic::config_keys::LICENSE_CHECK_LIC_CMD);
  const string_t license_file_name =
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

string_t LicenseChecker::make_machine_uid_cmd() {
  //[LICENSE] prod = 2
  //[LICENSE] make_lic_cmd = -g -s --prod
  //[FILES]	lic = D:\work\itvpn_setup\itvpn\bin\x64\lic.exe
  string_t license_process_path;
  license_process_path =
      PARSER::instance()->get_value(lic::config_keys::FILES_LIC);
  const string_t make_lic_cmd =
      PARSER::instance()->get_value(lic::config_keys::LICENSE_MAKE_LIC_CMD);
  const string_t license_prod =
      PARSER::instance()->get_value(lic::config_keys::LICENSE_PROD);
  const string_t license_uid =
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

bool LicenseChecker::check_update_day() {
  ACE_Date_Time date_time;
  // ACE_DEBUG(
  //    (LM_DEBUG, ACE_TEXT("%T (%t):\t\tLicenseChecker: Day of today - %d\n"),
  //    date_time.day()));
  const string_t license_update_day =
      PARSER::instance()->get_value(lic::config_keys::LICENSE_DAY_FOR_UPDATE);
  if (license_update_day.empty())
    throw std::runtime_error("Key of LICENSE.day_for_update is failed");
  const long day = ACE_OS::atol(license_update_day.c_str());
  return (date_time.day() >= day &&
          date_time.day() <= day + lic::constants::CHECK_DAYS);
}

bool LicenseChecker::check_license_day() {
  ACE_Date_Time date_time;
  // ACE_DEBUG(
  //	(LM_DEBUG, ACE_TEXT("%T (%t):\t\tLicenseChecker: Day of today - %d\n"),
  // date_time.day()));
  const string_t license_check_day = PARSER::instance()->get_value(
      lic::config_keys::LICENSE_DAY_FOR_CHECK_LIC);
  if (license_check_day.empty())
    throw std::runtime_error("Key of LICENSE.day_for_check_lic is failed");
  const long day = ACE_OS::atol(license_check_day.c_str());
  return (date_time.day() == day);
}

ACE_Date_Time LicenseChecker::current_license_date() {
  const string_t lic_file_name =
      PARSER::instance()->get_value(lic::config_keys::FILES_LIC_FILE_NAME);
  const string_t license = read_license_from_file(lic_file_name);
  return extract_license_date(license);
}

ACE_Date_Time LicenseChecker::extract_license_date(const string_t &lic) {
  ACE_Date_Time date_time = ACE_Date_Time(0);
  stringstream_t ss(lic);
  string_t item;
  std::vector<string_t> splitted;
  while (std::getline(ss, item, _XPLATSTR('.')))
    splitted.push_back(item);
  if (!splitted[2].empty()) {
    const std::vector<unsigned char> date_lic =
        utility::conversions::from_base64(splitted[2]);
    std::string str(date_lic.begin(), date_lic.end());
    int year, month, day;
#ifdef _WIN32
    swscanf_s(utility::conversions::to_string_t(str).c_str(),
              _XPLATSTR("%d-%d-%d"), &year, &month, &day);
#else // !_WIN32
    sscanf(str.c_str(), "%d-%d-%d", &year, &month, &day);
#endif
    date_time = ACE_Date_Time(day, month, year);
  }
  /*std::string item;
  std::vector<std::string> splitted;
  std::stringstream ss(utility::conversions::utf16_to_utf8(lic));
  while (std::getline(ss, item, '.'))
    splitted.push_back(item);
  size_t decode_len = 0;
  ACE_Auto_Basic_Array_Ptr<ACE_Byte> decode_buf(
      ACE_Base64::decode((const ACE_Byte *)splitted[2].c_str(), &decode_len));
  int year, month, day;
  sscanf((char *)decode_buf.get(), "%d-%d-%d", &year, &month, &day);
  ACE_Date_Time date_time(day, month, year);*/

  return date_time;
}

std::shared_ptr<LicenseExtractor>
LicenseChecker::make_license_extractor(const int64_t &attempt) {
  const web::http::uri address_ =
      PARSER::instance()->get_value(lic::config_keys::LICENSE_SRV_URI);
  // get unp
  const string_t unp =
      PARSER::instance()->get_value(lic::config_keys::LICENSE_UNP);
  // get agent
  const string_t agent =
      PARSER::instance()->get_value(lic::config_keys::LICENSE_AGENT_ID);
  // generate machine uid
  const string_t uid = generate_machine_uid();
  // get host_type
  const string_t host_type =
      PARSER::instance()->get_value(lic::config_keys::LICENSE_PROD);
  const Message message_ = Message(uid, unp, agent, host_type);
  return std::make_shared<LicenseExtractor>(address_, message_, attempt);
}
