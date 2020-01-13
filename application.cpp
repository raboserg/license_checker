// application.cpp

#include "stdafx.h"

#define LOGIN_CONNECT                                                                 \
  TM("/P7.Sink=Baical /P7.Pool=32768 /P7.PSize=65536 /P7.Addr=127.0.0.1 "      \
     "/P7:Port=9010")
#ifdef _WIN32
static const string LIC_INI_FILE = "lic_check_w.ini";
#else
static const string LIC_INI_FILE = "lic_check_l.ini";
#endif

enum {
  error_create_lic = 2,
};

static const std::string process_ = "/lic -v --lic ";
// use us singleton
static Parser parser_(LIC_INI_FILE);

static IP7_Client *l_iClient = P7_Create_Client(LOGIN_CONNECT);

string input_handle();
int worker(void *args) { return 0; }

int main(int argc, const char *argv[]) {

 
	for (int i = 0; i != 10; i++) {
    IP7_Trace *l_iTrace = P7_Create_Trace(l_iClient, TM("TraceChannel"));
    l_iTrace->P7_TRACE(0, TM("$$$$ %%%%% ^^^^^^^^Test trace message #%d"), 0);
  }

  std::string license_process_path = parser_.get_value("CONFIG.lic");
  const std::string license_file_name =
      parser_.get_value("FILES.lic_file_name");

  if (license_process_path.empty()) {
    std::cout << "ERROR: Not found license of path." << std::endl;
  } else if (!license_process_path.empty()) {

    LicenseChecker licenseChecker_;
    try {
      bool result = licenseChecker_.check_license(
          license_process_path.append(process_).append(license_file_name));
      if (result) {
        // create client
      } else {
      }
    } catch (system_error se) {
      if (se.code().value() == error_create_lic) {
        std::cout << se.what() << std::endl;
        return -1;
      }
    } catch (const char *msg) {
      std::cout << msg << std::endl;
    }
  }

#ifdef _WIN32
  WinNT::Start_Service();
#else
  LinuxNoficitator linuxNoficitator_;
  linuxNoficitator_.run_notify(argc, argv);
#endif

  std::cout << input_handle();
  return 0;
}

string input_handle() {
  std::string cmd;
  cout << "Input <Enter> for close" << endl;
  getline(std::cin, cmd);
  return cmd;
}
