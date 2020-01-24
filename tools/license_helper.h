#ifndef LICENSE_HELPER_H
#define LICENSE_HELPER_H

#include <constants.h>
#include <tracer.h>

#ifdef _WIN32
static const utility::string_t LIC_INI_FILE = U("lic_check_w.ini");
#else
static const utility::string_t LIC_INI_FILE = U("lic_check_l.ini");
#endif

namespace lic {
struct license_helper {

  static utility::string_t make_verify_license_cmd() {
    //[LICENSE] check_lic_cmd = -v --lic
    //[FILES] lic_file_name = license_item.lic
    //[FILES] lic=D:\work\itvpn_setup\itvpn\bin\x64\lic.exe
    const std::unique_ptr<Parser> parser_ =
        std::make_unique<Parser>(LIC_INI_FILE);
    utility::string_t license_process_path = parser_->get_value(U("FILES.lic"));
    const utility::string_t check_lic_cmd =
        parser_->get_value(U("LICENSE.check_lic_cmd"));
    const utility::string_t license_file_name =
        parser_->get_value(U("FILES.lic_file_name"));

    if (license_process_path.empty() && check_lic_cmd.empty() &&
        license_file_name.empty()) {
      throw std::runtime_error("Not create command varify license");
    }

    license_process_path.append(U(" "))
        .append(check_lic_cmd)
        .append(U(" "))
        .append(license_file_name);

    INFO_LOG(license_process_path.c_str());

    return license_process_path;
  }

  static utility::string_t make_machine_uid_cmd() {
    //[LICENSE] prod = 2
    //[LICENSE] make_lic_cmd = -g -s --prod
    //[FILES]	lic = D:\work\itvpn_setup\itvpn\bin\x64\lic.exe
    const std::unique_ptr<Parser> parser_ =
        std::make_unique<Parser>(LIC_INI_FILE);
    utility::string_t license_process_path;
    license_process_path = parser_->get_value(U("FILES.lic"));
    const utility::string_t make_lic_cmd =
        parser_->get_value(U("LICENSE.make_lic_cmd"));
    const utility::string_t license_prod =
        parser_->get_value(U("LICENSE.prod"));
    const utility::string_t license_uid =
        parser_->get_value(U("LICENSE.license_uid"));

    if (license_process_path.empty() && make_lic_cmd.empty() &&
        license_prod.empty()) {
      throw std::runtime_error("Not create command generate machine uid");
    }

    license_process_path.append(U(" "))
        .append(make_lic_cmd)
        .append(U(" "))
        .append(license_prod);

    INFO_LOG(license_process_path.c_str());

    return license_process_path;
  }

  //static web::json::value make_request_message() {
  //  const std::unique_ptr<Parser> parser_ =
  //      std::make_unique<Parser>(LIC_INI_FILE);

  //  const std::unique_ptr<LicenseChecker> licenseChecker_ =
  //      std::make_unique<LicenseChecker>();

  //  // get mac
  //  const utility::string_t mac =
  //      parser_->get_value(lic::config_file_keys::LICENSE_MAC);
  //  // get unp
  //  const utility::string_t unp = parser_->get_value(U("LICENSE.unp"));
  //  // generate machine uid
  //  const utility::string_t uid =
  //      licenseChecker_->generate_machine_uid(make_machine_uid_cmd());

  //  INFO_LOG(uid.c_str());

  //  web::json::value message;
  //  message[U("unp")] = web::json::value::string(unp);
  //  message[U("request")] = web::json::value::string(U("request"));
  //  message[U("mac")] = web::json::value::string(mac);
  //  return message;
  //}
};
} // namespace lic
#endif