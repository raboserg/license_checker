#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "stdafx.h"

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

class http_server {
public:
  http_server() {}
  http_server(const utility::string_t url);

  pplx::task<void> open() { return m_listener.open(); }
  pplx::task<void> close() { return m_listener.close(); }

private:
  void handle_get(http_request message);
  void handle_put(http_request message);
  void handle_post(http_request message);
  void handle_delete(http_request message);

  http_listener m_listener;
};

#endif // HTTP_SERVER_H
