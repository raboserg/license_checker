// application.cpp

#include "stdafx.h"

static const std::string process_ = "/lic -v --lic ";
// use us singleton
static Parser parser_("itvpn.ini");

string input_handle() {
  std::string cmd;
  cout << "Input <Enter> for close" << endl;
  getline(std::cin, cmd);
  return cmd;
}

int worker(void *args) { return 0; }

int main(int argc, const char *argv[]) {
  // boost::shared_ptr<std::thread> thread(new
  // std::thread(boost::bind(&http::server::server::run, &the_server)));

  std::string license_process_path = parser_.get_value("CONFIG.lic");
  std::string license_file_name = parser_.get_value("FILES.lic_file_name");

  if (!license_process_path.empty()) {
    LicenseChecker licenseChecker_;
    try {
      bool result = licenseChecker_.check_license(
          license_process_path.append(process_).append(license_file_name));
      if (result) {
        // create client
      } else {
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

  // std::unique_ptr<char *> ip = itvpn::ip_helper::get_ip();
  ////char *ip = itvpn::ip_helper::get_ip();
  // if (ip != nullptr) {
  //  argv[1] = (*ip);
  //  cout << "local IP: " << ip << endl;
  //}

  //  std::unique_ptr<std::thread> the_thread(
  //      new std::thread([argc, argv]() { run_server(argc, argv); }));
  //  the_thread->detach();

  std::cout << input_handle();
  return 0;
}
