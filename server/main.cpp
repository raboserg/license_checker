//
// main.cpp
//

#include "server.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <string>

int run_server(int argc, char *argv[]) {
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
    http::server::server s(argv[1], argv[2], argv[3]);

    // Run the server until stopped.
    s.run();
  } catch (std::exception &e) {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}
