#ifndef HANDLE_REQUEST_H
#define HANDLE_REQUEST_H

#include "mime_types.h"
#include <boost/asio/strand.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <iostream>

namespace beast = boost::beast;   // from <boost/beast.hpp>
namespace http = beast::http;     // from <boost/beast/http.hpp>
namespace net = boost::asio;      // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

class request_handler {
  // Append an HTTP rel-path to a local filesystem path.
  // The returned path is normalized for the platform.
  std::string path_cat(beast::string_view base, beast::string_view path) {
    if (base.empty())
      return std::string(path);
    std::string result(base);
#ifdef BOOST_MSVC
    char constexpr path_separator = '\\';
    if (result.back() == path_separator)
      result.resize(result.size() - 1);
    result.append(path.data(), path.size());
    for (auto &c : result)
      if (c == '/')
        c = path_separator;
#else
    char constexpr path_separator = '/';
    if (result.back() == path_separator)
      result.resize(result.size() - 1);
    result.append(path.data(), path.size());
#endif
    return result;
  }

public:
  // This function produces an HTTP response for the given
  // request. The type of the response object depends on the
  // contents of the request, so the interface requires the
  // caller to pass a generic lambda for receiving the response.
  template <class Body, class Allocator, class Send>
  void handle_request(beast::string_view doc_root,
                      http::request<Body, http::basic_fields<Allocator>> &&req,
                      Send &&send);
};

#endif // HANDLE_REQUEST_H
