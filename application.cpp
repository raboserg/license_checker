// application.cpp

#include "application.h"
#include "server.hpp"
#include <boost/program_options.hpp>

using namespace std;
using namespace boost;
namespace po = boost::program_options;

// static http::server::server *the_server = nullptr; // GLOBAL

int run_server(const int argc, const char *argv[]) {
  try {
    // Check command line arguments.
    if (argc != 4) {
      std::cerr << "Usage: http_server <address> <port> <doc_root>\n";
      std::cerr << "  For IPv4, try:\n";
      std::cerr << "    receiver 0.0.0.0 80 .\n";
      std::cerr << "  For IPv6, try:\n";
      std::cerr << "    receiver 0::0 80 .\n";
      return 1;
    }
    // Initialise the server.
    http::server::server server_(argv[1], argv[2], argv[3]);
    server_.run();
  } catch (std::exception &e) {
    std::cerr << "exception: " << e.what() << "\n";
  }
  return 0;
}

int main(int argc, const char *argv[]) {

  // po::options_description desc{"Options"};
  // desc.add_options()("help,h", "Print help message")(
  //    "locale,l", po::value<std::string>()->default_value("C"),
  //    "Locale settings (e.g. en-US.UTF8, zh-CN) ")(
  //    "file,f", po::value<std::string>()->default_value("itvpn.ini"),
  //    "The test .ini file");

  // po::variables_map vm;
  // try {
  //  po::store(po::parse_command_line(argc, argv, desc), vm);
  //} catch (po::error &e) {
  //  /* Invalid options */
  //  std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
  //  std::cout << "Boost property_tree tester:" << std::endl
  //            << desc << std::endl;
  //  return 0;
  //}
  // if (vm.count("help")) {
  //  /* print usage */
  //  std::cout << "Boost property_tree tester:" << std::endl
  //            << desc << std::endl;
  //  return 0;
  //}

  // std::string iniFile = vm["file"].as<std::string>().c_str();
  // std::cout << "Read ini file: " << iniFile << std::endl;

  // boost::shared_ptr<std::thread> thread(new
  // std::thread(boost::bind(&http::server::server::run, &the_server)));

  const LisenceChecker lisenceChecker("itvpn.ini");
  std::cout << lisenceChecker.get_value("CONFIG.ip_proxy") << endl;

  std::unique_ptr<std::thread> the_thread(
      new std::thread([argc, argv]() { run_server(argc, argv); }));
  the_thread->detach();

  std::string cmd;
  getline(std::cin, cmd);

  std::cout << cmd;
  return 0;
}
