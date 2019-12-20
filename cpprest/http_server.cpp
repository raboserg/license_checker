#include "http_server.h"

http_server::http_server(const utility::string_t url) : m_listener(url) {
  m_listener.support(methods::GET, std::bind(&http_server::handle_get, this,
                                             std::placeholders::_1));
  m_listener.support(methods::PUT, std::bind(&http_server::handle_put, this,
                                             std::placeholders::_1));
  m_listener.support(methods::POST, std::bind(&http_server::handle_post, this,
                                              std::placeholders::_1));
  m_listener.support(methods::DEL, std::bind(&http_server::handle_delete, this,
                                             std::placeholders::_1));
}

void http_server::handle_get(http_request message) {
  ucout << message.to_string() << endl;
  // auto path = requestPath(message);
  const std::vector<std::string> path = {"service", "test"};
  if (!path.empty()) {
    if (path[0] == "service" && path[1] == "test") {
      auto response = json::value::object();
      response[U("version")] = json::value::string(U("0.1.1"));
      response[U("status")] = json::value::string(U("ready!"));
      message.reply(status_codes::OK, response);
    }
  }
  message.reply(status_codes::NotFound);
}

void http_server::handle_post(http_request message) {
  ucout << message.to_string() << endl;
  auto response = json::value::object();
  response[U("first_name")] = web::json::value::string(U("Rabo"));
  response[U("last_name")] = web::json::value::string(U("Serg"));
  response[U("id")] = web::json::value::string(U("777"));
  message.reply(status_codes::OK, response);
}

void http_server::handle_delete(http_request message) {
  ucout << message.to_string() << endl;
  message.reply(status_codes::OK);
}

void http_server::handle_put(http_request message) {
  ucout << message.to_string() << endl;
  message.reply(status_codes::OK);
}
