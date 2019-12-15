#include "listener.h"

listener::listener(net::io_context &ioc, tcp::endpoint endpoint,
                   std::shared_ptr<std::string const> const &doc_root)
    : ioc_(ioc), acceptor_(net::make_strand(ioc)), doc_root_(doc_root) {
  beast::error_code error_code;

  // Open the acceptor
  acceptor_.open(endpoint.protocol(), error_code);
  if (error_code) {
    fail(error_code, "open");
    return;
  }

  // Allow address reuse
  acceptor_.set_option(net::socket_base::reuse_address(true), error_code);
  if (error_code) {
    fail(error_code, "set_option");
    return;
  }

  // Bind to the server address
  acceptor_.bind(endpoint, error_code);
  if (error_code) {
    fail(error_code, "bind");
    return;
  }

  // Start listening for connections
  acceptor_.listen(net::socket_base::max_listen_connections, error_code);
  if (error_code) {
    fail(error_code, "listen");
    return;
  }
}

// Start accepting incoming connections
void listener::run() { do_accept(); }

void listener::do_accept() {
  // The new connection gets its own strand
  acceptor_.async_accept(
      net::make_strand(ioc_),
      beast::bind_front_handler(&listener::on_accept, shared_from_this()));
}

void listener::on_accept(beast::error_code error_code, tcp::socket socket) {
  if (error_code) {
    fail(error_code, "accept");
  } else {
    // Create the session and run it
    std::make_shared<session>(std::move(socket), doc_root_)->run();
  }

  // Accept another connection
  do_accept();
}
