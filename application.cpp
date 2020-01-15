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

static const utility::string_t process_ = U("/lic -v --lic ");

utility::string_t input_handle();

int worker(void *args) { return 0; }

void posix_death_signal(int signum) {
	Tracer::critical(TM("ABORTED"), __LINE__, __FILE__, __FUNCTION__);
  signal(signum, SIG_DFL); // resend signal
	Tracer::clear();
	ucout << input_handle();
  exit(3);
}
//////////////////////////////////////////////////////////
IP7_Client* Tracer::Client_ = nullptr;
IP7_Trace * Tracer::Tracer_ = nullptr;
//////////////////////////////////////////////////////////

int main(int argc, const char *argv[]) {
	
	Tracer::init();

  signal(SIGSEGV, posix_death_signal);

	std::unique_ptr<Parser> parser_ = std::make_unique<Parser>(LIC_INI_FILE);

	utility::string_t license_process_path = parser_->get_value(U("CONFIG.lic"));
	const utility::string_t license_file_name =
      parser_->get_value(U("FILES.lic_file_name"));

  if (license_process_path.empty()) {
		Tracer::warning(TM("Not found license of path"), __LINE__, __FILE__, __FUNCTION__);
  } else if (!license_process_path.empty()) {
		std::unique_ptr<LicenseChecker> licenseChecker_ = std::make_unique<LicenseChecker>();
    try {
      bool result = licenseChecker_->check_license(
          license_process_path.append(process_).append(license_file_name));
      if (result) {
        // create client
      } else {
      }
    } catch (std::system_error se) {
      if (se.code().value() == error_create_lic) {
        std::cout << se.what() << std::endl;
				/*std::unique_ptr<const wchar_t *> dfsdfd = license_checker::char_to_wchar(se.what());
				utility::string_t sfdsfd(dfsdfd.release());
				const wchar_t *fdsfd = dfsdfd.release();
				l_iTrace->P7_ERROR(l_hModule, &dfsdfd.release(), 0);*/
				wchar_t WBuf[100];
				mbstowcs(WBuf, se.what(), 99);
				Tracer::error(WBuf, __LINE__, __FILE__, __FUNCTION__);
        raise(SIGSEGV);
      }
    } catch (const utf16char *msg) {
      ucout << msg << std::endl;
      raise(SIGSEGV);
    }
  }

#ifdef _WIN32
  WinNT::Start_Service();
#else
  LinuxNoficitator linuxNoficitator_;
  linuxNoficitator_.run_notify(argc, argv);
#endif

  ucout << input_handle();
	
	Tracer::clear();

  return 0;
}

utility::string_t input_handle() {
	utility::string_t cmd;
  ucout << U("Input <Enter> for close") << std::endl;
  getline(ucin, cmd);
  return cmd;
}
