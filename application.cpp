// application.cpp

#include "application.h"

using namespace std;
//namespace po = boost::program_options;

// static http::server::server *the_server = nullptr; // GLOBAL

// int run_server(const int argc, const char **argv) {
//  try {
//    // Check command line arguments.
//    cout << argc << endl;
//    if (argc != 4) {
//      std::cerr << "Usage: http_server <address> <port> <doc_root>\n";
//      std::cerr << "  For IPv4, try:\n";
//      std::cerr << "    receiver 0.0.0.0 80 .\n";
//      std::cerr << "  For IPv6, try:\n";
//      std::cerr << "    receiver 0::0 80 .\n";
//      return 1;
//    }
//    // Initialise the server.
//    http::server::server server_(argv[1], argv[2], argv[3]);
//    server_.run();
//  } catch (std::exception &e) {
//    std::cerr << "exception: " << e.what() << "\n";
//  }
//  return 0;
//}

// void shutdown_server() {
//  // the_server->close().wait();
//  return;
//}

string input_handle() {
  std::string cmd;
  cout << "Input <Enter> for close" << endl;
  getline(std::cin, cmd);
  return cmd;
}

int main(int argc, const char *argv[]) {
  // boost::shared_ptr<std::thread> thread(new
  // std::thread(boost::bind(&http::server::server::run, &the_server)));

  const LisenceChecker lisenceChecker("itvpn.ini");
  std::cout << lisenceChecker.get_value("CONFIG.ip_proxy") << endl;

#ifdef _WIN32
  WinNT::Start_Service();
#else
  main_run();
#endif


  //std::unique_ptr<char *> ip = itvpn::ip_helper::get_ip();
  ////char *ip = itvpn::ip_helper::get_ip();
  //if (ip != nullptr) {
  //  argv[1] = (*ip);
  //  cout << "local IP: " << ip << endl;
  //}

  //  std::unique_ptr<std::thread> the_thread(
  //      new std::thread([argc, argv]() { run_server(argc, argv); }));
  //  the_thread->detach();

  std::cout << input_handle();
  return 0;
}
