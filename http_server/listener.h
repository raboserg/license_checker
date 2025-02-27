#ifndef LISTENER_H
#define LISTENER_H

#include "session.h"
#include <boost/asio/strand.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <iostream>

namespace beast = boost::beast;   // from <boost/beast.hpp>
namespace http = beast::http;     // from <boost/beast/http.hpp>
namespace net = boost::asio;      // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

// Accepts incoming connections and launches the sessions
class listener : public std::enable_shared_from_this<listener> {
  net::io_context &ioc_;
  tcp::acceptor acceptor_;
  std::shared_ptr<std::string const> doc_root_;

public:
  listener(net::io_context &ioc, tcp::endpoint endpoint,
           std::shared_ptr<std::string const> const &doc_root);

  // Start accepting incoming connections
  void run();

  // Report a failure
  void fail(beast::error_code ec, char const *what) {
    std::cerr << what << ": " << ec.message() << "\n";
  }

private:
  void do_accept();

  void on_accept(beast::error_code ec, tcp::socket socket);
};

#endif // LISTENER_H
