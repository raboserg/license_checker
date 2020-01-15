// application.cpp

#include "stdafx.h"
#include <signal.h>

#define LOGIN_CONNECT                                                          \
  TM("/P7.Sink=Baical /P7.Pool=32768 /P7.PSize=65536 /P7.Addr=127.0.0.1 "      \
     "/P7:Port=9009")

#ifdef _WIN32
static const utility::string_t LIC_INI_FILE = U("lic_check_w.ini");
#else
static const utility::string_t LIC_INI_FILE = U("lic_check_l.ini");
#endif

enum {
  error_create_lic = 2,
};

static const utility::string_t process_ = U("/lic -v --lic ");
// use us singleton
// static Parser parser_(LIC_INI_FILE);

static IP7_Client *l_iClient = nullptr;
static IP7_Trace *l_iTrace = nullptr;
static IP7_Trace::hModule l_hModule = nullptr;

utility::string_t input_handle();

int worker(void *args) { return 0; }

void posix_death_signal(int signum) {

	l_iTrace->P7_CRITICAL(l_hModule, TM("ABORTED"), 0);

  if (l_iTrace) {
    l_iTrace->Release();
    l_iTrace = nullptr;
  }

  if (l_iClient) {
    l_iClient->Release();
    l_iClient = nullptr;
  }

  signal(signum, SIG_DFL); // resend signal
  ucout << input_handle();
  exit(3);
}

static void create_tracer() {
	l_iClient = P7_Create_Client(LOGIN_CONNECT);
	P7_Client_Share(l_iClient, TM("LICENSE_CHECKER_CLN_LOG"));
	l_iTrace = P7_Create_Trace(l_iClient, TM("TraceChannel"));
	l_iTrace->Register_Thread(TM("Application"), 0);
	IP7_Trace::hModule l_hModule = nullptr;
	l_iTrace->Register_Module(TM("Main"), &l_hModule);
	
	P7_Trace_Share(l_iTrace, TM("LICENSE_CHECKER_TRC_LOG"));
}

int main(int argc, const char *argv[]) {

  signal(SIGSEGV, posix_death_signal);

	create_tracer();

  Parser parser_(LIC_INI_FILE);
  // input_handle();

	utility::string_t license_process_path = parser_.get_value(U("CONFIG.lic"));
	const utility::string_t license_file_name =
      parser_.get_value(U("FILES.lic_file_name"));

  if (license_process_path.empty()) {
    l_iTrace->P7_ERROR(l_hModule, TM("Not found license of path"), 0);
  } else if (!license_process_path.empty()) {

    LicenseChecker licenseChecker_;

    try {
      bool result = licenseChecker_.check_license(
          license_process_path.append(process_).append(license_file_name));
      if (result) {
        // create client
      } else {
      }
    } catch (std::system_error se) {
      if (se.code().value() == error_create_lic) {
        std::cout << se.what() << std::endl;

        wchar_t WBuf[100];
        mbstowcs(WBuf, se.what(), 99);
				std::unique_ptr<const wchar_t *> dfsdfd = license_checker::char_to_wchar(se.what());
				utility::string_t sfdsfd(std::move(dfsdfd.get));
				l_iTrace->P7_ERROR(l_hModule, dfsdfd.get, 0);

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
  return 0;
}

utility::string_t input_handle() {
	utility::string_t cmd;
  ucout << U("Input <Enter> for close") << std::endl;
  getline(ucin, cmd);
  return cmd;
}
