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

  if (line.length() != lic::constats::UID_SIZE)
    throw std::runtime_error("size of UID is not valid: " + line.length());
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

bool LicenseChecker::check_update_day() {
  ACE_Date_Time date_time;
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T (%t):\tDay of today %d day\n"), date_time.day()));
  const utility::string_t license_update_day =
      PARSER::instance()->get_value(lic::config_keys::LICENSE_DAY_FOR_UPDATE);
  if (license_update_day.empty())
    throw std::runtime_error("Key of LICENSE.day_for_update is failed");
  const long day = ACE_OS::atol(license_update_day.c_str());
  return (date_time.day() >= day &&
          date_time.day() <= day + lic::constats::CHECK_DAYS);
}

ACE_Date_Time
LicenseChecker::extract_license_date(const utility::string_t &lic) {
  ACE_Date_Time date_time = ACE_Date_Time(0);
  utility::stringstream_t ss(lic);
  utility::string_t item;
  std::vector<utility::string_t> splitted;
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
#else
    sscanf(str.c_str(), "%d-%d-%d", &year, &month, &day);
#endif // !_WIN32
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

std::shared_ptr<LicenseExtractor> LicenseChecker::make_license_extractor(const int64_t &attempt) {
  const web::http::uri address_ =
      PARSER::instance()->get_value(lic::config_keys::LICENSE_SRV_URI);
  // get unp
  const utility::string_t unp =
      PARSER::instance()->get_value(lic::config_keys::LICENSE_UNP);
  // get agent
  const utility::string_t agent =
      PARSER::instance()->get_value(lic::config_keys::LICENSE_AGENT_ID);
  // generate machine uid
  const utility::string_t uid = generate_machine_uid();

  const Message message_ = Message(uid, unp, agent);

  return std::make_shared<LicenseExtractor>(address_, message_, attempt);
}
