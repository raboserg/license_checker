#include "listener.h"
#include <algorithm>
#include <iostream>
#include <thread>
#include <vector>

namespace net = boost::asio;      // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

int main(int argc, char *argv[]) {

  static const char template_text[] = "{{%AUTOESCAPE context=\"HTML\"}}"
                                      "ERROR: {{FUNCTION}}({{ARGS}}) returned "
                                      "{{ERROR_CODE}}: {{ERROR_MESSAGE}}\n";

  std::map<std::string, size_t> animal_population{
      {"humans", 7000000000},
      {"chickens", 17863376000},
      {"camels", 24246291},
      {"sheep", 1086881528},
      /* ... */
  };

  // Check command line arguments.
  if (argc != 5) {
    std::cerr
        << "Usage: http-server-async <address> <port> <doc_root> <threads>\n"
        << "Example:\n"
        << "    http-server-async 0.0.0.0 8080 . 1\n";
    return EXIT_FAILURE;
  }
  auto const address = net::ip::make_address(argv[1]);
  auto const port = static_cast<unsigned short>(std::atoi(argv[2]));
  auto const root = std::make_shared<std::string>(argv[3]);
  auto const threads = std::max<int>(1, std::atoi(argv[4]));

  // The io_context is required for all I/O
  net::io_context ioc{threads};

  // Create and launch a listening port
  std::make_shared<listener>(ioc, tcp::endpoint{address, port}, root)->run();

  // Run the I/O service on the requested number of threads
  std::vector<std::thread> v;
  v.reserve(static_cast<unsigned int>(threads - 1));
  for (auto i = threads - 1; i > 0; --i)
    v.emplace_back([&ioc] { ioc.run(); });
  ioc.run();

  return EXIT_SUCCESS;
}
