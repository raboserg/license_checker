#include "license_checker.h"
#include "ace/OS_NS_stdio.h"
#include "parser_ini.h"
#include <constants.h>
#include <cpprest/asyncrt_utils.h>
#include <tracer.h>

//#include "ace/Auto_Ptr.h"
//#include "ace/Codecs.h"

#include "ace/Log_Msg.h"
#include "ace/OS_NS_stdlib.h"

using namespace itvpnagent;

LicenseChecker::LicenseChecker() {}

utility::string_t LicenseChecker::run_proc(const string_t &command) {
  bp::ipstream is;
  bp::child process(command, bp::std_out > is);
  process.wait();
  std::string line;
  std::getline(is, line);
  return utility::conversions::to_string_t(line);
}

bool LicenseChecker::find_file(const path &dir_path, const string_t &file_name,
                               path &path_found) {
  bool result = false;
  if (!exists(dir_path))
    return false;
  directory_iterator end_itr;
  for (directory_iterator itr(dir_path); itr != end_itr; ++itr) {
    if (is_directory(itr->status())) {
      /// NOTE: if need to find recurcive in child directories
      // if (find_file(itr->path(), file_name, path_found))
      // result = true;
    } else if (itr->path().leaf() == file_name) {
      path_found = itr->path();
      result = true;
    }
  }
  return result;
}

bool LicenseChecker::is_license_file() {
  const string_t license_file_path = PARSER::instance()->options().lic_file;
  // path file_path = PARSER::instance()->options().lic_files_path;
  path file_path;
  int pos = license_file_path.find_last_of(_XPLATSTR("\\"));
  if (pos != string_t::npos)
    file_path = license_file_path.substr(0, pos);
  else
    file_path = _XPLATSTR(".");
  const string_t file_name = PARSER::instance()->options().lic_file_name;
  const bool result = find_file(file_path, file_name, file_path);
  if (!result)
    ERROR_LOG((license_file_path + TM(" not found")).c_str());
  return result;
}

bool LicenseChecker::verify_license() {
  bool result = false;
  const string_t verify_license_cmd = make_verify_license_cmd();
  string_t line = run_proc(verify_license_cmd);
  if (line.empty()) {
    string error_msg = conversions::to_utf8string(verify_license_cmd);
    throw std::runtime_error(error_msg.append(" of output is empty"));
  } else {
    const string_t code = line.substr(0, line.find_first_of(_XPLATSTR(":")));
    if (code == _XPLATSTR("ERROR")) {
      ERROR_LOG((TM("State license: ") + line).c_str());
      result = false;
    } else if (code.compare(_XPLATSTR("SUCCESS"))) {
      INFO_LOG((TM("State license: ") + line).c_str());
      result = true;
    } else {
      string error_msg = conversions::to_utf8string(verify_license_cmd);
      throw std::runtime_error(error_msg.append(" returned invalid responce"));
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

bool LicenseChecker::save_license_to_file(string_t &license) {
  int result = false;
  const size_t len = license.length();
  if (len && (license.c_str()[len - 1] == 0x00))
    license.erase(len - 1);
  // save license to file
  const string_t lic_file_name = PARSER::instance()->options().lic_file;
  ofstream_t file(lic_file_name, std::ios::out); //|std::ofstream::binary
  if (file.is_open()) {
    file << license;
    file.close();
    result = true;
  }
  return result;
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
  // COMMAND: -v --uid-file license.uid --lic-file lice.lic --prod 2
  const Options options = PARSER::instance()->options();
  string_t license_process_path = options.lic_app;
  const string_t license_prod = options.prod;
  const string_t service_path = options.service_path;
  const string_t license_file_name = options.lic_file;
  const string_t uid_file_name = options.uid_file_name;
  ///@TODO
  char_t buffer[BUFSIZ];
  const size_t fmt_len = ACE_OS::sprintf(
      buffer,
      //_XPLATSTR("%s -v --uid-file %s --lic-file %s --prod %s --pub-file %s"),
      _XPLATSTR("%s -v --uid-file %s --lic-file %s --prod %s"),
      license_process_path.c_str(), uid_file_name.c_str(),
      // license_file_name.c_str(), license_prod.c_str(), key_pub_file.c_str());
      license_file_name.c_str(), license_prod.c_str());
  INFO_LOG(buffer);
  return string_t(buffer);
}

string_t LicenseChecker::make_machine_uid_cmd() {
  //[LICENSE] prod = 2
  //[LICENSE] uid_cmd = -g -s --prod
  //[FILES]	lic = D:\work\itvpn_setup\itvpn\bin\x64\lic.exe
  string_t license_process_path = PARSER::instance()->options().lic_app;
  const string_t uid_cmd = PARSER::instance()->options().make_uid_cmd;
  const string_t license_prod = PARSER::instance()->options().prod;
  const string_t make_uid_cmd =
      _XPLATSTR(" ") + uid_cmd + _XPLATSTR(" ") + license_prod;
  license_process_path.append(make_uid_cmd);
  INFO_LOG(license_process_path.c_str());
  return license_process_path;
}

bool LicenseChecker::is_license_update_day() {
  ACE_Date_Time date_time;
  const long day_license_update =
      PARSER::instance()->options().day_license_update;
  return (date_time.day() >= day_license_update &&
          date_time.day() <= (day_license_update + lic::constants::CHECK_DAYS));
}

bool LicenseChecker::is_license_check_day() {
  ACE_Date_Time date_time;
  const long license_check_day =
      PARSER::instance()->options().day_license_check;
  return (date_time.day() == license_check_day);
}

ACE_Date_Time LicenseChecker::current_license_date() {
  const string_t lic_file_name = PARSER::instance()->options().lic_file;
  const string_t license = read_license_from_file(lic_file_name);
  return extract_license_date(license);
}

ACE_Date_Time LicenseChecker::extract_license_date(const string_t &lic) {
  string_t item;
  stringstream_t ss(lic);
  std::vector<string_t> splitted;
  ACE_Date_Time date_time = ACE_Date_Time(0);
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
  // get address
  const web::http::uri address_ =
      PARSER::instance()->options().license_manager_uri;
  // get unp
  const string_t unp = PARSER::instance()->options().unp;
  // get agent
  const string_t agent = PARSER::instance()->options().agentId;
  // generate machine uid
  const string_t uid = generate_machine_uid();
  // get host_type
  const string_t host_type = PARSER::instance()->options().prod;
  const Request_License request_ = Request_License(uid, unp, agent, host_type);
  return std::make_shared<LicenseExtractor>(address_, request_, attempt);
}

bool LicenseChecker::is_check_licenses_months(const int &next_month) {
  const ACE_Date_Time current_license_date = this->current_license_date();
  return next_month > current_license_date.month();
}
