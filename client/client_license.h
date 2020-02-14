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

/*
{
        "userMessage": "Не пройдена валидация",
        "developerMessage": null,
         "fieldErrors": [
                {
                        "validationErrorName": "RELATED_ENTITY_NOT_FOUND",
                        "message": "unp='9999998881' :
RELATED_ENTITY_NOT_FOUND", "fieldName": "unp"
                }
        ],
        "nestedError": null
}
*/

class Errors {
  utility::string_t code;
  utility::string_t error_type_;
  utility::string_t user_message_;
  utility::string_t developer_message_;
  std::vector<utility::string_t> fiels_errors;
public:
	utility::string_t userMessage() { return user_message_; };
	void userMessage(utility::string_t user_message) { user_message_ = user_message; };
};

class HostLicense {
  int year_;
  int month_;
  utility::string_t license_;

public:
  int year() { return year_; }
  int month() { return month_; }
  utility::string_t license() { return license_; }

  void year(const int year) { year_ = year; }
  void month(const int month) { month_ = month; }
  void license(const utility::string_t &license) { license_ = license; }
};

class HostStatus {
  int id_;
  utility::string_t name_;

public:
  int id() { return id_; }
  void id(const int id) { id_ = id; }
  utility::string_t name() { return name_; }
  void name(const utility::string_t &name) { name_ = name; }
};

class Result {
  std::shared_ptr<Errors> errors_;
  unsigned short status_code_;
  utility::string_t message_;
  std::shared_ptr<HostLicense> host_license_;
  std::shared_ptr<HostStatus> host_status_;

public:
  Result()
      : host_license_(std::make_shared<HostLicense>()),
        host_status_(std::make_shared<HostStatus>()) {}

  std::shared_ptr<HostLicense> host_license() { return host_license_; }
  std::shared_ptr<HostStatus> host_status() { return host_status_; }

  std::shared_ptr<Errors> errors() { return errors_; }
  void errors(const std::shared_ptr<Errors> errors) { errors_ = errors; }

  utility::string_t message() { return message_; }
  void message(const utility::string_t &message) { message_ = message; }
  unsigned short status_code() { return status_code_; }
  void status_code(unsigned short status_code) { status_code_ = status_code; }
};

class Message {
  const utility::string_t uid_;
  const utility::string_t unp_;
  const utility::string_t agent_;

public:
  Message(const utility::string_t uid, const utility::string_t unp,
          const utility::string_t agent)
      : uid_(uid), unp_(unp), agent_(agent) {}
  utility::string_t get_uid() const { return uid_; }
  utility::string_t get_unp() const { return unp_; }
  utility::string_t get_agent() const { return agent_; }
  bool is_valid() { return !uid_.empty() && !unp_.empty() && !agent_.empty(); }
};

class stage_handler : public web::http::http_pipeline_stage {
public:
  stage_handler() : m_Count(0) {}
  virtual pplx::task<web::http::http_response>
  propagate(web::http::http_request request) {
    INFO_LOG(request.to_string().c_str());
    // request.headers().set_content_type(_XPLATSTR("modified content type"));
    auto currentStage = this->shared_from_this();
    return next_stage()->propagate(request).then(
        [currentStage](
            web::http::http_response response) -> web::http::http_response {
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
  LicenseExtractor(const web::http::uri &address_, const Message &message_,
                   const int64_t &attempt);

  utility::string_t processing_license();
  std::shared_ptr<Result> get_result() { return result_; }

private:
  std::shared_ptr<Result> result_;
  Message message_;
  const int64_t attempt_;
  const web::http::uri address_;
  web::http::http_request request_;
  web::http::client::http_client client_;
  web::http::http_response send_request();
  web::http::client::http_client_config
  make_client_config(const int64_t &attempt);
  web::json::value make_request_message(const Message message_);
  void processing_errors(const web::http::http_response &response);
};

#endif
