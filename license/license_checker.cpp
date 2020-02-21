#include "license_checker.h"
//#include "license.h"
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

// std::string read_pub(const std::string &name) {
//  std::string pub;
//  ifstream file(name, std::ios::in);
//  if (file.is_open()) {
//    file >> pub;
//    file.close();
//  }
//  return pub;
//}
//
// std::vector<unsigned char> read_file(const std::string &fname) {
//  std::ifstream f(fname, std::fstream::binary | std::ios_base::ate);
//  if (!f)
//    return std::vector<unsigned char>();
//  auto sz = f.tellg();
//  std::vector<unsigned char> v;
//  v.resize(static_cast<std::vector<unsigned char>::size_type>(sz));
//  f.seekg(0);
//  f.rdbuf()->sgetn(reinterpret_cast<char *>(v.data()), v.size());
//  return v;
//}

bool LicenseChecker::verify_license() {
  bool result = false;

  // std::string uid_file = conversions::utf16_to_utf8(
  //    PARSER::instance()->get_value(lic::config_keys::LICENSE_UID));

  // std::vector<unsigned char> vuid;
  // if (uid_file.size()) {
  //  vuid = read_file(uid_file);
  //  if (vuid.empty()) {
  //    std::cout << "ERROR: Can't read uid file" << std::endl;

  //    const std::string uid =
  //        conversions::utf16_to_utf8(generate_machine_uid());
  //    vuid = std::vector<unsigned char>(uid.data(), uid.data() + uid.size());
  //    if (vuid.empty()) {
  //      std::cout << "ERROR: Can't read uid file" << std::endl;
  //      throw std::runtime_error("UID is empty");
  //    }
  //  }
  //}

  // unsigned int prod = _wtoi(
  //    PARSER::instance()->get_value(lic::config_keys::LICENSE_PROD).c_str());
  // std::string license = conversions::utf16_to_utf8(license_in);
  // std::vector<unsigned char> vlic = std::vector<unsigned char>(
  //    license_in.data(), license_in.data() + license_in.size());
  // std::vector<unsigned char> pub = read_file("lic_test_pub.bin");
  //// std::string pub = read_pub("lic_test_pub.bin");

  // int ret = lic_verify(vuid.data(), vuid.size(), pub.data(), pub.size(),
  // prod,
  //                     vlic.data(), vlic.size());

  string_t line = run_proc(make_verify_license_cmd());
  if (line.empty()) {
    throw std::runtime_error("lic of output is empty");
  } else {
    const string_t code = line.substr(0, line.find_first_of(_XPLATSTR(":")));
    INFO_LOG(line.c_str());
    if (code == _XPLATSTR("ERROR")) {
      result = false;
    } else if (code.compare(_XPLATSTR("SUCCESS"))) {
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
  ofstream_t file(lic_file_name, std::ios::out); //|std::ofstream::binary
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
  //[LICENSE]	prod = 2
  //[FILES] lic_file_name = license_item.lic
  //[FILES] lic=D:\work\itvpn_setup\itvpn\bin\x64\lic.exe
  //[FILES] uid_file_name = license.uid
  // COMMAND: -v --uid - file license.uid --lic - file lice.lic --prod 2
  const string_t service_path = PARSER::instance()->get_service_path();
  // TODO: lic.exe can be to other folder
  string_t license_process_path =
      PARSER::instance()->get_value(lic::config_keys::FILES_LIC);
  const string_t license_prod =
      PARSER::instance()->get_value(lic::config_keys::LICENSE_PROD);
  const string_t license_file_name =
      PARSER::instance()->get_value(lic::config_keys::FILES_LIC_FILE_NAME);
  const string_t uid_file_name =
      PARSER::instance()->get_value(lic::config_keys::FILES_UID_FILE_NAME);

  if (license_prod.empty())
    throw std::runtime_error("License prod is empty");
  if (license_file_name.empty())
    throw std::runtime_error("License file name is empty");
  if (uid_file_name.empty())
    throw std::runtime_error("Uid file name is empty");
  if (license_process_path.empty())
    throw std::runtime_error("License process path is empty");

  const string_t check_lic_cmd = _XPLATSTR("-v --uid-file ") + service_path +
                                 uid_file_name + _XPLATSTR(" --lic-file ") +
                                 service_path + license_file_name +
                                 _XPLATSTR(" --prod ") + license_prod;

  license_process_path.append(_XPLATSTR(" ")).append(check_lic_cmd);

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
      PARSER::instance()->get_value(lic::config_keys::LICENSE_MAKE_UID_CMD);
  const string_t license_prod =
      PARSER::instance()->get_value(lic::config_keys::LICENSE_PROD);
  // const string_t license_uid =
  // PARSER::instance()->get_value(lic::config_keys::FILES_UID_FILE_NAME);

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

bool LicenseChecker::is_license_update_day() {
  ACE_Date_Time date_time;
  const string_t license_update_day =
      PARSER::instance()->get_value(lic::config_keys::CONFIG_DAY_LICENSE_UPDATE);
  if (license_update_day.empty())
    throw std::runtime_error("Key of LICENSE.day_for_update is failed");
  const long day = ACE_OS::atol(license_update_day.c_str());
  return (date_time.day() >= day &&
          date_time.day() <= (day + lic::constants::CHECK_DAYS));
}

bool LicenseChecker::is_license_check_day() {
  ACE_Date_Time date_time;
  const string_t license_check_day = PARSER::instance()->get_value(
      lic::config_keys::CONFIG_DAY_LICENSE_CHECK);
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
