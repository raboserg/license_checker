﻿#include "client_license.h"
#include <constants.h>
#include <cpprest/asyncrt_utils.h>

using namespace std;
using namespace utility;
using namespace conversions;

LicenseExtractor::LicenseExtractor(const uri &address,
                                   const Request_License &req_lic,
                                   const int64_t &attempt)
    : attempt_(attempt), client_(address, make_client_config(attempt)),
      request_license_(req_lic) {
  result_ = make_shared<Result>();
  http_request_.set_method(methods::POST);
  http_request_.set_body(make_request_message(req_lic).serialize(),
                         web::http::details::mime_types::application_json);
  //???crossplat::threadpool::initialize_with_threads(10);
}

shared_ptr<Result> LicenseExtractor::processing_license() {
  const http_response response = send_request();
  result_->status_code(response.status_code());
  if (result_->status_code() == status_codes::OK) {
    response.content_ready().wait();
    http_headers headers = response.headers();
    const string_t content_type = headers[header_names::content_type];
    if (content_type != http::details::mime_types::application_json) {
      const shared_ptr<Errors> errors = make_shared<Errors>();
      errors->error_type(http::details::mime_types::text_plain_utf8);
      result_->errors(errors);
      throw web::http::http_exception(
          lic::error_code::MIME_TYPES,
          _XPLATSTR("ERROR: server returned ") +
              http::details::mime_types::text_plain_utf8);
    }
    value json_value = response.extract_json().get();
    result_->message(json_value[_XPLATSTR("message")].as_string());
    if (!json_value[_XPLATSTR("hostStatus")].is_null()) {
      const object host_status_json =
          json_value[_XPLATSTR("hostStatus")].as_object();
      const shared_ptr<HostStatus> host_status = make_shared<HostStatus>();
      host_status->id(host_status_json.at(_XPLATSTR("id")).as_integer());
      host_status->name(host_status_json.at(_XPLATSTR("name")).as_string());
      result_->host_status(host_status);
      if (result_->host_status()->id() == lic::lic_host_status::ACTIVE) {
        if (!json_value[_XPLATSTR("hostLicense")].is_null()) {
          value license_json = json_value[_XPLATSTR("hostLicense")];
          const shared_ptr<HostLicense> host_license =
              make_shared<HostLicense>();
          if (!license_json[_XPLATSTR("license")].is_null())
            host_license->license(
                license_json[_XPLATSTR("license")].as_string());
          if (!license_json[_XPLATSTR("month")].is_null())
            host_license->month(license_json[_XPLATSTR("month")].as_integer());
          if (!license_json[_XPLATSTR("year")].is_null())
            host_license->year(license_json[_XPLATSTR("year")].as_integer());
          result_->host_license(host_license);
        }
      }
      return result_;
    }
  } else {
    processing_http_errors(response);
  }
}

void LicenseExtractor::processing_http_errors(const http_response &response) {
  string_t error = utility::conversions::to_string_t(
      "Fault connection: status code - " + to_string(response.status_code()));

  const shared_ptr<Errors> errors = make_shared<Errors>();
  errors->developer_message(error);
  if (response.status_code() == status_codes::UnprocessableEntity) {
    response.content_ready().wait();
    value json_value = response.extract_json().get();

    errors->userMessage(json_value[_XPLATSTR("userMessage")].as_string());
    error.append(_XPLATSTR(" ")).append(errors->userMessage());

    if (!json_value[_XPLATSTR("fieldErrors")].is_null()) {
      auto field_errors = json_value[_XPLATSTR("fieldErrors")].as_array();
      for (auto field : field_errors) {
        const object field_error = field.as_object();
        FieldError field_error_;
        field_error_.message = field_error.at(_XPLATSTR("message")).as_string();
        field_error_.validationErrorName =
            field_error.at(_XPLATSTR("validationErrorName")).as_string();
        field_error_.fieldName =
            field_error.at(_XPLATSTR("fieldPath")).as_string();
        errors->add_error(field_error_);

        error.append(_XPLATSTR(", fieldPath: "))
            .append(field_error_.fieldName)
            .append(_XPLATSTR(" "))
            .append(field_error_.message);
      }
      // ERROR_LOG(error.c_str());
    }
    result_->errors(errors);
  }
  if (response.status_code() == status_codes::BadGateway) {
    result_->errors(errors);
  }
  throw runtime_error(to_utf8string(error).c_str());
}

client::http_client_config
LicenseExtractor::make_client_config(const int64_t &attempt) {
  client::http_client_config config;
  config.set_validate_certificates(false);
  config.set_timeout(std::chrono::seconds(60));
  return config;
}

http_response LicenseExtractor::send_request() {
  const chrono::seconds time_try_connection_{attempt_};
  //???	std::chrono::duration_cast<std::chrono::seconds>(attempt_);
  if (!request_license_.is_valid())
    throw runtime_error("message for request is empty");

  shared_ptr<http_pipeline_stage> countStage = make_shared<stage_handler>();
  client_.add_handler(countStage);

  http_response response;
  auto start = chrono::steady_clock::now();
  while (true) {
    try {
      response = client_.request(http_request_).get();
      break;
    } catch (const http_exception &ex) {
      ucout << _XPLATSTR("Http error code: ") << ex.error_code().value()
            << std::endl; // error code win = 12029,
                          // error code lin = 110 - Request canceled by user.
      if (chrono::steady_clock::now() > (start + time_try_connection_)) {
        ERROR_LOG(to_string_t(ex.what()).c_str());
        //????? throw_with_nested(runtime_error(ex.what()));
        const shared_ptr<Errors> errors = make_shared<Errors>();
        errors->code(ex.error_code().value());
        result_->errors(errors);
        throw ex;
      }
    }
  }
  return response;
}

value LicenseExtractor::make_request_message(const Request_License request) {
  value message;
  message[_XPLATSTR("unp")] = value::string(request.get_unp());
  message[_XPLATSTR("request")] = value::string(request.get_uid());
  message[_XPLATSTR("agentId")] = value::string(request.get_agent());
  message[_XPLATSTR("hostTypeId")] = value::string(request.get_host_type());
  INFO_LOG(message.serialize().c_str());
  return message;
}
