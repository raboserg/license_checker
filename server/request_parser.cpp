//
// request_parser.cpp
//

#include "request_parser.hpp"

namespace http {
namespace server {

request_parser::request_parser() : state_(method_start) {}

void request_parser::reset() { state_ = method_start; }

request_parser::result_type request_parser::consume(request &req, char input) {
  switch (state_) {
  case method_start:
    if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
      return bad;
    } else {
      state_ = method;
      req.method.push_back(input);
      return indeterminate;
    }
  case method:
    if (input == ' ') {
      state_ = uri;
      return indeterminate;
    } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
      return bad;
    } else {
      req.method.push_back(input);
      return indeterminate;
    }
  case uri:
    if (input == ' ') {
      state_ = http_version_h;
      return indeterminate;
    } else if (is_ctl(input)) {
      return bad;
    } else {
      req.uri.push_back(input);
      return indeterminate;
    }
  case http_version_h:
    if (input == 'H') {
      state_ = http_version_t_1;
      return indeterminate;
    } else {
      return bad;
    }
  case http_version_t_1:
    if (input == 'T') {
      state_ = http_version_t_2;
      return indeterminate;
    } else {
      return bad;
    }
  case http_version_t_2:
    if (input == 'T') {
      state_ = http_version_p;
      return indeterminate;
    } else {
      return bad;
    }
  case http_version_p:
    if (input == 'P') {
      state_ = http_version_slash;
      return indeterminate;
    } else {
      return bad;
    }
  case http_version_slash:
    if (input == '/') {
      req.http_version_major = 0;
      req.http_version_minor = 0;
      state_ = http_version_major_start;
      return indeterminate;
    } else {
      return bad;
    }
  case http_version_major_start:
    if (is_digit(input)) {
      req.http_version_major = req.http_version_major * 10 + input - '0';
      state_ = http_version_major;
      return indeterminate;
    } else {
      return bad;
    }
  case http_version_major:
    if (input == '.') {
      state_ = http_version_minor_start;
      return indeterminate;
    } else if (is_digit(input)) {
      req.http_version_major = req.http_version_major * 10 + input - '0';
      return indeterminate;
    } else {
      return bad;
    }
  case http_version_minor_start:
    if (is_digit(input)) {
      req.http_version_minor = req.http_version_minor * 10 + input - '0';
      state_ = http_version_minor;
      return indeterminate;
    } else {
      return bad;
    }
  case http_version_minor:
    if (input == '\r') {
      state_ = expecting_newline_1;
      return indeterminate;
    } else if (is_digit(input)) {
      req.http_version_minor = req.http_version_minor * 10 + input - '0';
      return indeterminate;
    } else {
      return bad;
    }
  case expecting_newline_1:
    if (input == '\n') {
      state_ = header_line_start;
      return indeterminate;
    } else {
      return bad;
    }
  case header_line_start:
    if (input == '\r') {
      state_ = expecting_newline_3;
      return indeterminate;
    } else if (!req.headers.empty() && (input == ' ' || input == '\t')) {
      state_ = header_lws;
      return indeterminate;
    } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
      return bad;
    } else {
      req.headers.push_back(header());
      req.headers.back().name.push_back(input);
      state_ = header_name;
      return indeterminate;
    }
  case header_lws:
    if (input == '\r') {
      state_ = expecting_newline_2;
      return indeterminate;
    } else if (input == ' ' || input == '\t') {
      return indeterminate;
    } else if (is_ctl(input)) {
      return bad;
    } else {
      state_ = header_value;
      req.headers.back().value.push_back(input);
      return indeterminate;
    }
  case header_name:
    if (input == ':') {
      state_ = space_before_header_value;
      return indeterminate;
    } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
      return bad;
    } else {
      req.headers.back().name.push_back(input);
      return indeterminate;
    }
  case space_before_header_value:
    if (input == ' ') {
      state_ = header_value;
      return indeterminate;
    } else {
      return bad;
    }
  case header_value:
    if (input == '\r') {
      state_ = expecting_newline_2;
      return indeterminate;
    } else if (is_ctl(input)) {
      return bad;
    } else {
      req.headers.back().value.push_back(input);
      return indeterminate;
    }
  case expecting_newline_2:
    if (input == '\n') {
      state_ = header_line_start;
      return indeterminate;
    } else {
      return bad;
    }
  case expecting_newline_3:
    return (input == '\n') ? good : bad;
  default:
    return bad;
  }
}

int content_length(const request &request) {
  int length = 0;
  const std::string name = "Content-Length";
  const std::vector<header>::const_iterator item =
      std::find_if(request.headers.begin(), request.headers.end(),
                   [&](const auto &header_) { return header_.name == name; });
  if (item != request.headers.end())
    length = std::atoi(item->value.c_str());
  return length;
}

void request_parser::parse_paremeters(request &request,
                                      const std::string &params) const {
  if (!params.empty()) {
    const int length = content_length(request);
    if (length > 0) {
      std::string post_parameters(std::begin(params),
                                  std::begin(params) + length);
      std::vector<std::string> pairs;
      boost::split(pairs, post_parameters, boost::is_any_of("&"));
      for (auto pair : pairs) {
        std::vector<std::string> values;
        boost::split(values, pair, boost::is_any_of("="));
        request.parameters.push_back(header());
        request.parameters.back().name = values[0];
        request.parameters.back().value = values[1];
      }
    }
  }
}

bool request_parser::is_char(int c) { return c >= 0 && c <= 127; }

bool request_parser::is_ctl(int c) { return (c >= 0 && c <= 31) || (c == 127); }

bool request_parser::is_digit(int c) { return c >= '0' && c <= '9'; }

bool request_parser::is_tspecial(int c) {
  switch (c) {
  case '(':
  case ')':
  case '<':
  case '>':
  case '@':
  case ',':
  case ';':
  case ':':
  case '\\':
  case '"':
  case '/':
  case '[':
  case ']':
  case '?':
  case '=':
  case '{':
  case '}':
  case ' ':
  case '\t':
    return true;
  default:
    return false;
  }
}
} // namespace server
} // namespace http
