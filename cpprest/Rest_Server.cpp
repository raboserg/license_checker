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

#include "stdafx.h"

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

class BlackJackDealer {
public:
  BlackJackDealer() {}
  BlackJackDealer(utility::string_t url);

  pplx::task<void> open() { return m_listener.open(); }
  pplx::task<void> close() { return m_listener.close(); }

private:
  void handle_get(http_request message);
  void handle_put(http_request message);
  void handle_post(http_request message);
  void handle_delete(http_request message);

  http_listener m_listener;
};

std::unique_ptr<BlackJackDealer> g_httpDealer;

void on_initialize(const string_t &address) {
  // Build our listener's URI from the configured address and the hard-coded
  // path "blackjack/dealer"

  uri_builder uri(address);
  uri.append_path(U("./"));

  auto addr = uri.to_uri().to_string();
  g_httpDealer = std::unique_ptr<BlackJackDealer>(new BlackJackDealer(addr));
  g_httpDealer->open().wait();

  ucout << utility::string_t(U("Listening for requests at: ")) << addr
        << std::endl;

  return;
}

void on_shutdown() {
  g_httpDealer->close().wait();
  return;
}


//void TEST_FIXTURE(uri_address, set_body_vector) {
//  http_listener listener(m_uri);
//  listener.open().wait();
//  test_http_client::scoped_client client(m_uri);
//  test_http_client *p_client = client.client();
//
//  listener.support([&](http_request request) {
//    http_asserts::assert_request_equals(request, methods::POST, U("/"));
//    http_response response(status_codes::OK);
//    std::vector<unsigned char> v_body;
//    v_body.push_back('A');
//    v_body.push_back('B');
//    v_body.push_back('C');
//    response.set_body(std::move(v_body));
//    request.reply(response).wait();
//  });
//  VERIFY_ARE_EQUAL(0, p_client->request(methods::POST, U("")));
//  p_client->next_response()
//      .then([&](test_response *p_response) {
//        http_asserts::assert_test_response_equals(p_response, status_codes::OK,
//                                                  U("application/octet-stream"),
//                                                  U("ABC"));
//      })
//      .wait();
//
//  listener.close().wait();
//}

//
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
  utility::string_t port = U("34568");
  utility::string_t address = U("http://localhost:");

  ucout << port.c_str() << ", " << address.c_str() << std::endl;

  if (argc == 3) {
    address = argv[1];
    port = argv[2];
  }

  address.append(port);

  on_initialize(address);
  std::cout << "Press ENTER to exit." << std::endl;

  std::string line;
  std::getline(std::cin, line);

  on_shutdown();
  return 0;
}