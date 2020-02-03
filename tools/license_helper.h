#ifndef LICENSE_HELPER_H
#define LICENSE_HELPER_H

#include <constants.h>
#include <tracer.h>
#include <parser_ini.h>
#include "license_checker.h"
#include <cpprest/asyncrt_utils.h>
#include <cpprest/details/basic_types.h>
#include <cpprest/json.h>

//#undef U

#ifdef _WIN32
static const utility::string_t LIC_INI_FILE = U("lic_check_w.ini");
#else
static const utility::string_t LIC_INI_FILE = U("lic_check_l.ini");
#endif

namespace lic {
struct license_helper {
  
	//TODO: Move to license_checker
	static utility::string_t make_verify_license_cmd() {
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
    utility::string_t license_process_path;
    license_process_path = PARSER::instance()->get_value(lic::config_keys::FILES_LIC);
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

    license_process_path.append(U(" "))
        .append(make_lic_cmd)
        .append(U(" "))
        .append(license_prod);

    INFO_LOG(license_process_path.c_str());

    return license_process_path;
  }

	// @TODO: Move to anything factory 
	static web::json::value make_request_message() {
		const std::unique_ptr<LicenseChecker> licenseChecker_ =			std::make_unique<LicenseChecker>();
		// get unp
		const utility::string_t unp =
			PARSER::instance()->get_value(lic::config_keys::LICENSE_UNP);
		// get mac
		const utility::string_t mac =
			PARSER::instance()->get_value(lic::config_keys::LICENSE_AGENT_ID);
		// generate machine uid
		const utility::string_t uid = licenseChecker_->generate_machine_uid();
		INFO_LOG(uid.c_str());
		web::json::value message;
		message[U("unp")] = web::json::value::string(unp);
		message[U("request")] = web::json::value::string(uid);
		message[U("agentId")] = web::json::value::string(mac);
		return message;
	}
};
} // namespace lic
#endif
