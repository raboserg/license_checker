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

// typedef utils::Singleton<Parser> PARSER;

void posix_death_signal(int signum) {
  CRITICAL(0, TM("ABORTED"));
  signal(signum, SIG_DFL); // resend signal
  ucout << input_handle();
  exit(3);
}

void check_lic() {
  const std::unique_ptr<Parser> parser_ =
      std::make_unique<Parser>(LIC_INI_FILE);
  const std::unique_ptr<LicenseChecker> licenseChecker_ =
      std::make_unique<LicenseChecker>();
  /*[license]
    prod = 2
    files_path = d:\work\itvpn_setup\itvpn\bin\x64\
                 make_lic_cmd = -g --prod 2 --uid - file
     check_lic_cmd = -v --lic
     [files]
     lic =d:\work\itvpn_setup\itvpn\bin\x64\lic.exe
                 [FILES]
                        lic=D:\work\itvpn_setup\itvpn\bin\x64\lic.exe*/
  utility::string_t license_process_path;
  try {
    license_process_path = parser_->get_value(U("FILES.lic"));
    const utility::string_t check_lic_cmd =
        parser_->get_value(U("LICENSE.check_lic_cmd"));
    const utility::string_t license_file_name =
        parser_->get_value(U("FILES.lic_file_name"));
    if (license_process_path.empty() && check_lic_cmd.empty() &&
        license_file_name.empty()) {
      ERRORS(0, TM("Not make license of path"));
      raise(SIGSEGV);
    }
    license_process_path.append(U(" "))
        .append(check_lic_cmd)
        .append(U(" "))
        .append(license_file_name);
    if (licenseChecker_->check_license(license_process_path)) {
      // create client
      INFO(0, TM("Create client"));
    }
  } catch (boost::exception &ex) {
    utility::string_t str_utf16 =
        utility::conversions::to_string_t(boost::diagnostic_information(ex));
    WARNING(0, str_utf16.c_str());
    raise(SIGSEGV);
  } catch (std::system_error se) {
    if (se.code().value() == error_create_lic) {
      utility::string_t str_utf16 =
          utility::conversions::to_string_t(se.what());
      WARNING(0, str_utf16.c_str());
      raise(SIGSEGV);
    }
  } catch (const utf16char *msg) {
    ucout << msg << std::endl;
    raise(SIGSEGV);
  }
}

void make_file_lic() {
  // make_lic_cmd = -g --prod 2 --uid - file
  // prod = 2
  //[FILES]	lic = D:\work\itvpn_setup\itvpn\bin\x64\lic.exe* /
  const std::unique_ptr<Parser> parser_ =
      std::make_unique<Parser>(LIC_INI_FILE);
  const std::unique_ptr<LicenseChecker> licenseChecker_ =
      std::make_unique<LicenseChecker>();

  utility::string_t license_process_path;
  license_process_path = parser_->get_value(U("FILES.lic"));

  const utility::string_t make_lic_cmd =
      parser_->get_value(U("LICENSE.make_lic_cmd"));

  const utility::string_t license_prod = parser_->get_value(U("LICENSE.prod"));

  license_process_path.append(U(" "))
      .append(make_lic_cmd)
      .append(U(" "))
      .append(license_prod);
  utility::string_t dsfdsf =
      licenseChecker_->make_file_license(license_process_path);
  INFO(0, TM("Create client"));
}

int main(int argc, const char *argv[]) {

  signal(SIGSEGV, posix_death_signal);

  // make_file_lic();
  check_lic();

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
