#include "application.h"
//#include "stdafx.h"
#include <license_helper.h>
#include <signal.h>

//#include <pplx/threadpool.h>

enum {
  error_create_lic = 2, // mybe create process
};

utility::string_t input_handle();

void posix_death_signal(int signum) {
  //  const std::error_category fdsfsd = utility::details::linux_category();
  //  std::cout << fdsfsd << std::endl;
  CRITICAL_LOG(TM("ABORTED"));
  signal(signum, SIG_DFL); // resend signal
  ucout << input_handle();
  lic::os_utilities::sleep(1000);
  exit(3);
}

void license_worker() {
  try {
    const std::unique_ptr<LicenseChecker> licenseChecker_ =
        std::make_unique<LicenseChecker>();
    // verify license_item
    const bool is_license_update = licenseChecker_->verify_license_file();
    if (is_license_update) {
      // generate machine uid
      const utility::string_t machine_uid =
          licenseChecker_->generate_machine_uid();
      INFO_LOG(machine_uid.c_str());

      DEBUG_LOG(TM("Need to get machine MAC"));
      //////////////////////////////////////
      // Need to create client here
      //////////////////////////////////////
    } else {
      INFO_LOG(TM("License is SUCCESS"));
      // end thread
    }
  } catch (const boost::exception &ex) {
    utility::string_t str_utf16 =
        utility::conversions::to_string_t(boost::diagnostic_information(ex));
    ERROR_LOG(str_utf16.c_str());
    raise(SIGSEGV);
  } catch (const std::system_error se) {
    if (se.code().value() == error_create_lic) {
      utility::string_t str_utf16 =
          utility::conversions::to_string_t(se.what());
      ERROR_LOG(str_utf16.c_str());
      raise(SIGSEGV);
    }
  } catch (const std::exception &msg) {
    ERROR_LOG(utility::conversions::to_string_t(msg.what()).c_str());
    raise(SIGSEGV);
  }
}

static web::json::value make_request_message() {
  const std::unique_ptr<Parser> parser_ =
      std::make_unique<Parser>(LIC_INI_FILE);
  const std::unique_ptr<LicenseChecker> licenseChecker_ =
      std::make_unique<LicenseChecker>();
  // get unp
  const utility::string_t unp =
      parser_->get_value(lic::config_keys::LICENSE_UNP);
  // get mac
  const utility::string_t mac =
      parser_->get_value(lic::config_keys::LICENSE_MAC);
  // generate machine uid
  const utility::string_t uid = licenseChecker_->generate_machine_uid();
	ucout << uid << std::endl;
  INFO_LOG(uid.c_str());
  web::json::value message;
  message[U("unp")] = web::json::value::string(unp);
  message[U("request")] = web::json::value::string(uid);
  message[U("mac")] = web::json::value::string(mac);
  return message;
}

int main(int argc, const char *argv[]) {

  setlocale(LC_ALL, "ru_RU.UTF-8");
  signal(SIGSEGV, posix_death_signal);

  const std::unique_ptr<LicenseChecker> licenseChecker_ =
      std::make_unique<LicenseChecker>();

  try {

		const web::http::uri address_ =
			PARSER::instance()->get_value(lic::config_keys::LICENSE_SRV_URI);

		const std::unique_ptr<LicenseExtractor> licenseExtractor_ =
			std::make_unique<LicenseExtractor>(address_, make_request_message(), 5);

    utility::string_t lic = licenseExtractor_->receive_license();
    if (!lic.empty()) {
      licenseChecker_->save_license_to_file(lic);
    }
  } catch (const std::runtime_error &err) {
    ERROR_LOG(utility::conversions::to_string_t(err.what()).c_str());
    raise(SIGSEGV);
  }

  // lic::os_utilities::sleep(1000);
#ifdef _WIN32
  WinNT::Start_Service();
#else
  LinuxNoficitator linuxNoficitator_;
  linuxNoficitator_.run_notify(argc, argv);

#endif
  ucout << input_handle();
  return 0;
}

utility::string_t input_handle() {
  utility::string_t cmd;
  ucout << U("Input <Enter> for close") << std::endl;
  getline(ucin, cmd);
  return cmd;
}
