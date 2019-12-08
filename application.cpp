// application.cpp

#include "application.h"
#include <boost/program_options.hpp>

using namespace std;
using namespace boost;
using namespace boost::process;
namespace po = boost::program_options;

int main(int argc, const char *argv[]) {

  po::options_description desc{"Options"};
  desc.add_options()("help,h", "Print help message")(
      "locale,l", po::value<std::string>()->default_value("C"),
      "Locale settings (e.g. en-US.UTF8, zh-CN) ")(
      "file,f", po::value<std::string>()->default_value("itvpn.ini"),
      "The test .ini file");

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
  } catch (po::error &e) {
    /* Invalid options */
    std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
    std::cout << "Boost property_tree tester:" << std::endl
              << desc << std::endl;
    return 0;
  }
  if (vm.count("help")) {
    /* print usage */
    std::cout << "Boost property_tree tester:" << std::endl
              << desc << std::endl;
    return 0;
  }

  std::string iniFile = vm["file"].as<std::string>().c_str();
  std::cout << "Read ini file: " << iniFile << std::endl;

  const LisenceChecker lisenceChecker(iniFile);
  cout << lisenceChecker.get_value("CONFIG.ip_proxy") << endl;

  auto ovpn_path = boost::process::search_path("itvpn.ini", {""});
  cout << ovpn_path << endl;

  ipstream pipe_stream;
  child c("gcc --version", std_out > pipe_stream);

  std::string line;

  while (pipe_stream && std::getline(pipe_stream, line) && !line.empty())
    std::cerr << line << std::endl;

  c.wait();
}
