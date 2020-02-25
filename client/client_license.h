// client_license.h
#pragma once
#ifndef CLIENT_LICENSE_H
#define CLIENT_LICENSE_H

#define _TURN_OFF_PLATFORM_STRING
#include "constants.h"
#include "tracer.h"
#include <cpprest/details/basic_types.h>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <vector>

using namespace web;
using namespace http;
using namespace json;

struct FieldError {
  utility::string_t message;
  utility::string_t fieldName;
  utility::string_t validationErrorName;
};

class Errors {
  int code_;
  utility::string_t error_type_;
  utility::string_t user_message_;
  utility::string_t developer_message_;
  std::vector<FieldError> field_errors;

public:
  int code() const { return code_; }
  void code(const int code) { code_ = code; }

  utility::string_t error_type() const { return error_type_; }
  void error_type(const utility::string_t error_type) {
    error_type_ = error_type;
  }
  utility::string_t userMessage() const { return user_message_; }
  void userMessage(const utility::string_t user_message) {
    user_message_ = user_message;
  }

  void developer_message(const utility::string_t developer_message) {
    developer_message_ = developer_message;
  }
  utility::string_t developer_message() const { return developer_message_; }

  std::vector<FieldError> fiels_errors() const { return field_errors; }
  void add_error(FieldError error) { field_errors.push_back(error); }
  FieldError &operator[](const int index) { return field_errors[index]; }
};

class HostLicense {
  int year_;
  int month_;
  utility::string_t license_;

public:
  HostLicense() : year_(0), month_(0) {}
  int year() const { return year_; }
  int month() const { return month_; }
  utility::string_t license() const { return license_; }

  void year(const int year) { year_ = year; }
  void month(const int month) { month_ = month; }
  void license(const utility::string_t &license) { license_ = license; }
};

class HostStatus {
  int id_;
  utility::string_t name_;

public:
  int id() const { return id_; }
  void id(const int id) { id_ = id; }
  utility::string_t name() const { return name_; }
  void name(const utility::string_t &name) { name_ = name; }
};

class Result {
  std::shared_ptr<Errors> errors_;
  unsigned short status_code_;
  utility::string_t message_;
  std::shared_ptr<HostLicense> host_license_;
  std::shared_ptr<HostStatus> host_status_;

public:
  Result() {}

  std::shared_ptr<HostLicense> host_license() const { return host_license_; }

  std::shared_ptr<HostStatus> host_status() const { return host_status_; }

  void host_license(std::shared_ptr<HostLicense> host_license) {
    host_license_ = host_license;
  }

  void host_status(std::shared_ptr<HostStatus> host_status) {
    host_status_ = host_status;
  }

  std::shared_ptr<Errors> errors() const { return errors_; }
  void errors(const std::shared_ptr<Errors> errors) { errors_ = errors; }

  utility::string_t message() const { return message_; }
  void message(const utility::string_t &message) { message_ = message; }

  unsigned short status_code() const { return status_code_; }
  void status_code(unsigned short status_code) { status_code_ = status_code; }
};

class Message {
  const utility::string_t uid_;
  const utility::string_t unp_;
  const utility::string_t agent_;
  const utility::string_t host_type_;

public:
  Message(const utility::string_t uid, const utility::string_t unp,
          const utility::string_t agent, const utility::string_t host_type)
      : uid_(uid), unp_(unp), agent_(agent), host_type_(host_type) {}
  utility::string_t get_uid() const { return uid_; }
  utility::string_t get_unp() const { return unp_; }
  utility::string_t get_agent() const { return agent_; }
  utility::string_t get_host_type() const { return host_type_; }
  bool is_valid() {
    return !uid_.empty() && !unp_.empty() && !agent_.empty() &&
           !host_type_.empty();
  }
};

class stage_handler : public web::http::http_pipeline_stage {
public:
  stage_handler() : m_Count(0) {}
  virtual pplx::task<web::http::http_response> propagate(http_request request) {
    INFO_LOG(request.to_string().c_str());
    // request.headers().add(_XPLATSTR("User-Agent"),
    // _XPLATSTR("itVPNAgent/0.1"));
    request.headers()[_XPLATSTR("User-Agent")] = (_XPLATSTR("itVPNAgent/0.1"));
    // request.headers().operator[_XPLATSTR("User -
    // Agent")].set_content_type(_XPLATSTR("modified content type"));
    auto currentStage = this->shared_from_this();
    return next_stage()->propagate(request).then(
        [currentStage](http_response response) -> http_response {
          /*response.headers().add(_XPLATSTR("My Header"), data.str());*/
          INFO_LOG(response.to_string().c_str());
          return response;
        });
  }

private:
  int m_Count;
};

class LicenseExtractor {
public:
  LicenseExtractor(const uri &address_, const Message &message_,
                   const int64_t &attempt);

  std::shared_ptr<Result> processing_license();
  std::shared_ptr<Result> get_result() const { return result_; }
  uri get_uri() { return address_; }

private:
  std::shared_ptr<Result> result_;
  Message message_;
  const int64_t attempt_;
  const uri address_;
  http_request request_;
  client::http_client client_;
  http_response send_request();
  client::http_client_config make_client_config(const int64_t &attempt);
  value make_request_message(const Message message_);
  void processing_http_errors(const http_response &response);
};

#endif
