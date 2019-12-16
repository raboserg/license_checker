#ifndef SESSION_H
#define SESSION_H

#include "mime_types.h"
#include <boost/algorithm/string.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <fstream>
#include <iostream>

//#include "request_handler.h"
//#include "send_lambda.h"

namespace beast = boost::beast;   // from <boost/beast.hpp>
namespace http = beast::http;     // from <boost/beast/http.hpp>
namespace net = boost::asio;      // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

// Handles an HTTP server connection
class session : public std::enable_shared_from_this<session> {

  typedef http::request<std::shared_ptr<std::string const>> request;
  // This is the C++11 equivalent of a generic lambda.
  // The function object is used to send an HTTP message.
  struct send_lambda {
    session &self_;

    explicit send_lambda(session &self) : self_(self) {}

    template <bool isRequest, class Body, class Fields>
    void operator()(http::message<isRequest, Body, Fields> &&msg) const {
      // The lifetime of the message has to extend
      // for the duration of the async operation so
      // we use a shared_ptr to manage it.
      auto sp = std::make_shared<http::message<isRequest, Body, Fields>>(
          std::move(msg));

      // Store a type-erased version of the shared
      // pointer in the class to keep it alive.
      self_.res_ = sp;

      // Write the response
      http::async_write(
          self_.stream_, *sp,
          boost::beast::bind_front_handler(
              &session::on_write, self_.shared_from_this(), sp->need_eof()));
    }
  };

  // Append an HTTP rel-path to a local filesystem path.
  // The returned path is normalized for the platform.
  std::string path_cat(beast::string_view base, beast::string_view path);

  template <class Body, class Allocator, class Send>
  void handle_request(beast::string_view doc_root,
                      http::request<Body, http::basic_fields<Allocator>> &&req,
                      Send &&send);

  //  template <class Body, class Allocator, class Send>
  //  void handle_request(beast::string_view doc_root, request &&req, Send
  //  &&send);

  // Report a failure
  void fail(beast::error_code ec, char const *what) {
    std::cerr << what << ": " << ec.message() << "\n";
  }
  
  std::tuple<beast::error_code, std::string>
  load_html(const std::string request_path);

  beast::tcp_stream stream_;
  beast::flat_buffer buffer_;
  std::shared_ptr<std::string const> doc_root_;
  http::request<http::string_body> req_;
  std::shared_ptr<void> res_;
  send_lambda lambda_;
  // request_handler request_handler_;

public:
  // Take ownership of the stream
  session(tcp::socket &&socket,
          std::shared_ptr<std::string const> const &doc_root);

  // Start the asynchronous operation
  void run();

  void do_read();

  void on_read(beast::error_code ec, std::size_t bytes_transferred);

  void on_write(bool close, beast::error_code ec,
                std::size_t bytes_transferred);
  void do_close();
};

#endif // SESSION_H
