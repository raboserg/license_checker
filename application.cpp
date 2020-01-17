#include "stdafx.h"
#include <signal.h>

#ifdef _WIN32
static const utility::string_t LIC_INI_FILE = U("lic_check_w.ini");
#else
static const utility::string_t LIC_INI_FILE = U("lic_check_l.ini");
#endif

enum {
  error_create_lic = 2,
};

utility::string_t input_handle();

typedef utils::Singleton<Parser> PARSER;

void posix_death_signal(int signum) {
  CRITICAL_LOG(TM("ABORTED"));
  signal(signum, SIG_DFL); // resend signal
  ucout << input_handle();
  utilities::os_utilities::sleep(10000);
  exit(3);
}

utility::string_t create_verify_license_command() {
  //[LICENSE] check_lic_cmd = -v --lic
  //[FILES] lic_file_name = license.lic
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
    throw "Not create command varify license";
  }

  license_process_path.append(U(" "))
      .append(check_lic_cmd)
      .append(U(" "))
      .append(license_file_name);
  INFO_LOG(license_process_path.c_str());
  return license_process_path;
}

utility::string_t create_generate_machine_uid_command() {
  //[LICENSE] prod = 2
  //[LICENSE] make_lic_cmd = -g -s --prod
  //[FILES]	lic = D:\work\itvpn_setup\itvpn\bin\x64\lic.exe
  const std::unique_ptr<Parser> parser_ =
      std::make_unique<Parser>(LIC_INI_FILE);
  utility::string_t license_process_path;
  license_process_path = parser_->get_value(U("FILES.lic"));
  const utility::string_t make_lic_cmd =
      parser_->get_value(U("LICENSE.make_lic_cmd"));
  const utility::string_t license_prod = parser_->get_value(U("LICENSE.prod"));
  const utility::string_t license_uid =
      parser_->get_value(U("LICENSE.license_uid"));

  if (license_process_path.empty() && make_lic_cmd.empty() &&
      license_prod.empty()) {
    throw "Not create command generate machine uid";
  }

  license_process_path.append(U(" "))
      .append(make_lic_cmd)
      .append(U(" "))
      .append(license_prod);
  INFO_LOG(license_process_path.c_str());
  return license_process_path;
}

// void verify_license_file() {
//  try {
//    const std::unique_ptr<LicenseChecker> licenseChecker_ =
//        std::make_unique<LicenseChecker>();
//
//    const utility::string_t license_process_path =
//        create_verify_license_command();
//    if (licenseChecker_->verify_license_file(license_process_path)) {
//      // create client
//      INFO(0, TM("Need to create client here"));
//    } else {
//      INFO(0, TM("License is SUCCESS"));
//      // end thread
//    }
//  } catch (boost::exception &ex) {
//    utility::string_t str_utf16 =
//        utility::conversions::to_string_t(boost::diagnostic_information(ex));
//    ERRORS(0, str_utf16.c_str());
//    raise(SIGSEGV);
//  } catch (std::system_error se) {
//    if (se.code().value() == error_create_lic) {
//      utility::string_t str_utf16 =
//          utility::conversions::to_string_t(se.what());
//      ERRORS(0, str_utf16.c_str());
//      raise(SIGSEGV);
//    }
//  } catch (const utf16char *msg) {
//    ucout << msg << std::endl;
//    raise(SIGSEGV);
//  }
//}

// void generate_machine_uid() {
//  try {
//    const std::unique_ptr<LicenseChecker> licenseChecker_ =
//        std::make_unique<LicenseChecker>();
//
//    licenseChecker_->generate_machine_uid(
//        create_generate_machine_uid_command());
//
//  } catch (boost::exception &ex) {
//    utility::string_t str_utf16 =
//        utility::conversions::to_string_t(boost::diagnostic_information(ex));
//    ERRORS(0, str_utf16.c_str());
//    raise(SIGSEGV);
//  } catch (std::system_error se) {
//    if (se.code().value() == error_create_lic) {
//      utility::string_t str_utf16 =
//          utility::conversions::to_string_t(se.what());
//      ERRORS(0, str_utf16.c_str());
//      raise(SIGSEGV);
//    }
//  } catch (const utf16char *msg) {
//    ERRORS(0, msg);
//    raise(SIGSEGV);
//  }
//}

void license_worker() {
  try {
    const std::unique_ptr<LicenseChecker> licenseChecker_ =
        std::make_unique<LicenseChecker>();

    const utility::string_t license_process_path =
        create_verify_license_command();
    // verify license
    if (licenseChecker_->verify_license_file(license_process_path)) {
      // generate machine uid
      const utility::string_t machine_uid =
          licenseChecker_->generate_machine_uid(
              create_generate_machine_uid_command());
      INFO_LOG(machine_uid.c_str());
      DEBUG_LOG(TM("Need to get machine MAC"));

      // Need to create client here
    } else {
      INFO_LOG(TM("License is SUCCESS"));
      // end thread
    }
  } catch (boost::exception &ex) {
    utility::string_t str_utf16 =
        utility::conversions::to_string_t(boost::diagnostic_information(ex));
    ERROR_LOG(0, str_utf16.c_str());
    raise(SIGSEGV);
  } catch (std::system_error se) {
    if (se.code().value() == error_create_lic) {
      utility::string_t str_utf16 =
          utility::conversions::to_string_t(se.what());
      ERROR_LOG(str_utf16.c_str());
      raise(SIGSEGV);
    }
    //} catch (const utf16char *msg) {
  } catch (const std::exception &msg) {
    ERROR_LOG(utility::conversions::to_string_t(msg.what()).c_str());
    raise(SIGSEGV);
  }
}

int main(int argc, const char *argv[]) {

	ERROR_LOG(TM("ACE_ERRORACE_ERRORACE_ERRORACE_ERRORACE_ERRORACE_ERRORACE_ERRORACE_ERROR"));
  signal(SIGSEGV, posix_death_signal);

  license_worker();

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
