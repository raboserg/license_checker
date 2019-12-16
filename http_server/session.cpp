#include "session.h"
#include "handle_request.h"

session::session(tcp::socket &&socket,
                 std::shared_ptr<std::string const> const &doc_root)
    : stream_(std::move(socket)), doc_root_(doc_root), lambda_(*this) {}

// Start the asynchronous operation
void session::run() { do_read(); }

void session::do_read() {
  // Make the request empty before reading,
  // otherwise the operation behavior is undefined.
  req_ = {};

  // Set the timeout.
  stream_.expires_after(std::chrono::seconds(30));

  // Read a request
  http::async_read(
      stream_, buffer_, req_,
      beast::bind_front_handler(&session::on_read, shared_from_this()));
}

void session::on_read(beast::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  // This means they closed the connection
  if (ec == http::error::end_of_stream)
    return do_close();

  if (ec)
    return fail(ec, "read");

  // Send the response
  handle_request(*doc_root_, std::move(req_), lambda_);
}

void session::on_write(bool close, beast::error_code error_code,
                       std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  if (error_code)
    return fail(error_code, "write");

  if (close) {
    // This means we should close the connection, usually because
    // the response indicated the "Connection: close" semantic.
    return do_close();
  }

  // We're done with the response so delete it
  res_ = nullptr;

  // Read another request
  do_read();
}

void session::do_close() {
  // Send a TCP shutdown
  beast::error_code ec;
  stream_.socket().shutdown(tcp::socket::shutdown_send, ec);

  // At this point the connection is closed gracefully
}

// std::string session::path_cat(beast::string_view base,
//                              beast::string_view path) {
//  if (base.empty())
//    return std::string(path);
//  std::string result(base);
//#ifdef BOOST_MSVC
//  char constexpr path_separator = '\\';
//  if (result.back() == path_separator)
//    result.resize(result.size() - 1);
//  result.append(path.data(), path.size());
//  for (auto &c : result)
//    if (c == '/')
//      c = path_separator;
//#else
//  char constexpr path_separator = '/';
//  if (result.back() == path_separator)
//    result.resize(result.size() - 1);
//  result.append(path.data(), path.size());
//#endif
//  return result;
//}

// std::tuple<beast::error_code, std::string>
// session::load_html(const std::string request_path) {
//  // Determine the file extension.
//  std::size_t last_slash_pos = request_path.find_last_of("/");
//  std::size_t last_dot_pos = request_path.find_last_of(".");
//  std::string extension;
//  if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos) {
//    extension = request_path.substr(last_dot_pos + 1);
//  }
//  beast::error_code error_code;
//  std::ifstream is_(request_path.c_str(), std::ios::in | std::ios::binary);
//  if (!is_) {
//    //???error_code.assign(beast::errc::no_such_file_or_directory,
//    // error_code.category);
//    return std::make_tuple(error_code, std::string());
//  }
//  // Fill out the reply to be sent to the client.
//  char buf[512];
//  std::string content = std::string();
//  while (is_.read(buf, sizeof(buf)).gcount() > 0)
//    content.append(buf, is_.gcount());
//  return std::make_tuple(error_code, content);
//}

// template <class Body, class Allocator, class Send>
// inline void session::handle_request(
//    beast::string_view doc_root,
//    http::request<Body, http::basic_fields<Allocator>> &&req, Send &&send) {
//  // Returns a bad request response
//  auto const bad_request = [&req](beast::string_view why) {
//    http::response<http::string_body> res{http::status::bad_request,
//                                          req.version()};
//    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//    res.set(http::field::content_type, "text/html");
//    res.keep_alive(req.keep_alive());
//    res.body() = std::string(why);
//    res.prepare_payload();
//    return res;
//  };

//  // Returns a not found response
//  auto const not_found = [&req](beast::string_view target) {
//    http::response<http::string_body> res{http::status::not_found,
//                                          req.version()};
//    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//    res.set(http::field::content_type, "text/html");
//    res.keep_alive(req.keep_alive());
//    res.body() = "The resource '" + std::string(target) + "' was not found.";
//    res.prepare_payload();
//    return res;
//  };

//  // Returns a server error response
//  auto const server_error = [&req](beast::string_view what) {
//    http::response<http::string_body> res{http::status::internal_server_error,
//                                          req.version()};
//    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//    res.set(http::field::content_type, "text/html");
//    res.keep_alive(req.keep_alive());
//    res.body() = "An error occurred: '" + std::string(what) + "'";
//    res.prepare_payload();
//    return res;
//  };

//  // Make sure we can handle the method
//  if (req.method() != http::verb::get && req.method() != http::verb::post)
//    return send(bad_request("Unknown HTTP-method"));

//  // Request path must be absolute and not contain "..".
//  if (req.target().empty() || req.target()[0] != '/' ||
//      req.target().find("..") != beast::string_view::npos)
//    return send(bad_request("Illegal request-target"));

//  // Respond to GET request
//  if (req.method() == http::verb::get) {
//    std::cout << req_.body() << std::endl;
//    // Build the path to the requested file
//    std::string path = path_cat(doc_root, req.target());
//    if (req.target().back() == '/')
//      path.append("index.html");

//    // Attempt to open the file
//    beast::error_code error_code;
//    http::file_body::value_type body;
//    body.open(path.c_str(), beast::file_mode::scan, error_code);

//    // Handle the case where the file doesn't exist
//    if (error_code == beast::errc::no_such_file_or_directory)
//      return send(not_found(req.target()));

//    // Handle an unknown error
//    if (error_code)
//      return send(server_error(error_code.message()));

//    // Cache the size since we need it after the move
//    auto const size = body.size();

//    http::response<http::file_body> res{
//        std::piecewise_construct, std::make_tuple(std::move(body)),
//        std::make_tuple(http::status::ok, req.version())};
//    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//    res.set(http::field::content_type, mime_types::mime_type(path));
//    res.content_length(size);
//    res.keep_alive(req.keep_alive());
//    return send(std::move(res));
//  }

//  // Respond to POST request
//  if (req.method() == http::verb::post) {
//    std::cout << req_.body() << std::endl;
//    //???    std::cout << boost::beast::buffers_to_string(req_.body().data())
//    //              << std::endl;
//    std::vector<std::string> pairs;
//    boost::split(pairs, req_.body(), boost::is_any_of("&"));
//    std::map<std::string, std::string> parameters;
//    for (auto pair : pairs) {
//      std::vector<std::string> values;
//      boost::split(values, pair, boost::is_any_of("="));
//      parameters.insert(
//          std::pair<std::string, std::string>(values[0], values[1]));
//    }

//    // http::string_body::value_type body;
//    std::for_each(
//        parameters.begin(), parameters.end(),
//        [](const std::map<std::string, std::string>::value_type &ite) {
//          std::cout << ite.first << " " << ite.second << std::endl;
//          std::string pair_{ite.first + "=" + ite.second};
//        });

//    std::string path = path_cat(doc_root, req.target());
//    if (req.target().back() == '/')
//      path.append("index.html");

//    std::string content;
//    beast::error_code error_code;
//    std::tie(error_code, content) = load_html(path);

//    http::response<http::string_body> res{
//        std::piecewise_construct, std::make_tuple(std::move(content)),
//        std::make_tuple(http::status::ok, req.version())};
//    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//    //    req.method(beast::http::verb::get);
//    //    req.target("/");
//    return send(std::move(res));
//  }
//}
