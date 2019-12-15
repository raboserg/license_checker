/***
 * Copyright (C) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE.txt file in the project root for
 *full license information.
 *
 * =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 *
 * BlackJack_Servr.cpp - Simple server application for blackjack
 *
 * =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 ****/

#include "http_server.h"

static std::unique_ptr<http_server> g_http_server;

void on_initialize(const string_t &address) {
  // Build our listener's URI from the configured address and the hard-coded
  // path "blackjack/dealer"
  uri_builder uri(address);
  uri.append_path(U("/home/user/res"));

  auto addr = uri.to_uri().to_string();
  g_http_server = std::unique_ptr<http_server>(new http_server(addr));
  g_http_server->open().wait();

  ucout << utility::string_t(U("Listening for requests at: ")) << addr
        << std::endl;

  return;
}

void on_shutdown() {
  g_http_server->close().wait();
  return;
}

// To start the server, run the below command with admin privileges:
// BlackJack_Server.exe <port>
// If port is not specified, will listen on 34568
//
#ifdef _WIN32
int wmain(int argc, wchar_t *argv[])
#else
int main(int argc, char *argv[])
#endif
{
  utility::string_t port = U("9090");
  utility::string_t address = U("http://localhost");

  ucout << address.c_str() << ", " << port.c_str() << std::endl;

  if (argc == 3) {
    address = argv[1];
    port = argv[2];
  }

  address.append(":" + port);

  on_initialize(address);
  std::cout << "Press ENTER to exit." << std::endl;

  std::string line;
  std::getline(std::cin, line);

  on_shutdown();
  return 0;
}
