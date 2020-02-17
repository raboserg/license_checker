#include "client_license.h"
#include <constants.h>
#include <cpprest/asyncrt_utils.h>

using namespace std;
using namespace utility;
using namespace conversions;

LicenseExtractor::LicenseExtractor(const uri &address, const Message &message,
                                   const int64_t &attempt)
    : client_(address, make_client_config(attempt)), message_(message),
      attempt_(attempt) {
  result_ = make_shared<Result>();
  request_.set_method(methods::POST);
  request_.set_body(make_request_message(message).serialize(),
                    web::http::details::mime_types::application_json);
}

string_t LicenseExtractor::processing_license() {
  const http_response response = send_request();
  result_->status_code(response.status_code());
  if (result_->status_code() == status_codes::OK) {
    response.content_ready().wait();
    http_headers headers = response.headers();
    const string_t content_type = headers[header_names::content_type];
    // text/html;charset=UTF-8
    value json_value = response.extract_json().get();
    result_->message(json_value[_XPLATSTR("message")].as_string());
    if (!json_value[_XPLATSTR("hostStatus")].is_null()) {
      const object host_status_json =
          json_value[_XPLATSTR("hostStatus")].as_object();
      result_->host_status()->id(
          host_status_json.at(_XPLATSTR("id")).as_integer());
      result_->host_status()->name(
          host_status_json.at(_XPLATSTR("name")).as_string());
      if (result_->host_status()->id() == lic::lic_host_status::ACTIVE) {
        if (!json_value[_XPLATSTR("hostLicense")].is_null()) {
          value license_json = json_value[_XPLATSTR("hostLicense")];
          result_->host_license()->license(
              license_json[_XPLATSTR("license")].as_string());
          result_->host_license()->month(
              license_json[_XPLATSTR("month")].as_integer());
          result_->host_license()->year(
              license_json[_XPLATSTR("year")].as_integer());
        }
      } else
        result_->host_license() = nullptr; //???
      return result_->host_license()->license();
    }
  } else {
    processing_errors(response);
  }
}

void LicenseExtractor::processing_errors(const http_response &response) {
  string error =
      "Fault connection: status code - " + to_string(response.status_code());
  ERROR_LOG(to_string_t(error).c_str());

  const shared_ptr<Errors> errors = make_shared<Errors>();

  if (response.status_code() == status_codes::UnprocessableEntity) {
    response.content_ready().wait();
    value json_value = response.extract_json().get();

    errors->userMessage(json_value[_XPLATSTR("userMessage")].as_string());
    error.append(" ").append(to_utf8string(errors->userMessage()));

    ERROR_LOG(errors->userMessage().c_str());

    if (!json_value[_XPLATSTR("fieldErrors")].is_null()) {
      auto field_errors = json_value[_XPLATSTR("fieldErrors")].as_array();
      for (auto field : field_errors) {
        const object field_error = field.as_object();
        FieldError field_error_;
        field_error_.message = field_error.at(_XPLATSTR("message")).as_string();
        field_error_.validationErrorName =
            field_error.at(_XPLATSTR("validationErrorName")).as_string();
        field_error_.fieldName =
            field_error.at(_XPLATSTR("fieldName")).as_string();
        errors->add_error(field_error_);

        ERROR_LOG(string_t(_XPLATSTR("fieldName "))
                      .append(field_error_.fieldName)
                      .append(_XPLATSTR(" "))
                      .append(field_error_.message)
                      .c_str());
      }
    }
  }
  result_->errors(errors);
  throw runtime_error(error.c_str());
}

client::http_client_config
LicenseExtractor::make_client_config(const int64_t &attempt) {
  client::http_client_config config;
  config.set_validate_certificates(false);
  return config;
}

http_response LicenseExtractor::send_request() {
  const chrono::seconds time_try_connection_{attempt_};
  //???	std::chrono::duration_cast<std::chrono::seconds>(attempt_);
  if (!message_.is_valid())
    throw runtime_error("message for request is empty");

  shared_ptr<http_pipeline_stage> countStage = make_shared<stage_handler>();
  client_.add_handler(countStage);

  auto start = chrono::steady_clock::now();
  while (true) {
    try {
      return client_.request(request_).get();
      break;
    } catch (http_exception &ex) {
      ucout << ex.error_code().value() << std::endl; // error code = 12029
      if (chrono::steady_clock::now() > (start + time_try_connection_)) {
        ERROR_LOG(to_string_t(ex.what()).c_str());
        throw_with_nested(runtime_error(ex.what()));
      }
    }
  }
}

value LicenseExtractor::make_request_message(const Message message_) {
  value message;
  message[_XPLATSTR("unp")] = value::string(message_.get_unp());
  message[_XPLATSTR("request")] = value::string(message_.get_uid());
  message[_XPLATSTR("agentId")] = value::string(message_.get_agent());
	message[_XPLATSTR("hostType")] = value::string(message_.get_host_type());
  INFO_LOG(message.serialize().c_str());
  return message;
}
