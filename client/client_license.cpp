#include "client_license.h"
#include <constants.h>
#include <cpprest/asyncrt_utils.h>

LicenseExtractor::LicenseExtractor(const web::http::uri &address,
                                   const Message &message,
                                   const int64_t &attempt)
    : client_(address, make_client_config(attempt)), message_(message),
      attempt_(attempt) {
  result_ = std::make_shared<Result>();
  request_.set_method(web::http::methods::POST);
  request_.set_body(make_request_message(message).serialize(),
                    web::http::details::mime_types::application_json);
}

utility::string_t LicenseExtractor::processing_license() {
  const web::http::http_response response = send_request();
  result_->status_code(response.status_code());
  if (result_->status_code() == web::http::status_codes::OK) {
    response.content_ready().wait();
    web::http::http_headers headers = response.headers();
    const utility::string_t content_type =
        headers[web::http::header_names::content_type];
    // text/html;charset=UTF-8
    web::json::value json_value = response.extract_json().get();
    result_->message(json_value[_XPLATSTR("message")].as_string());
    if (!json_value[_XPLATSTR("hostStatus")].is_null()) {
      const web::json::object host_status_json =
          json_value[_XPLATSTR("hostStatus")].as_object();
      result_->host_status()->id(
          host_status_json.at(_XPLATSTR("id")).as_integer());
      result_->host_status()->name(
          host_status_json.at(_XPLATSTR("name")).as_string());
      if (result_->host_status()->id() == lic::host_states::ACTIVE) {
        if (!json_value[_XPLATSTR("hostLicense")].is_null()) {
          web::json::value license_json = json_value[_XPLATSTR("hostLicense")];
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

void LicenseExtractor::processing_errors(
    const web::http::http_response &response) {
  std::string error = "Fault connection: status code - " +
                      std::to_string(response.status_code());
  ERROR_LOG(utility::conversions::to_string_t(error).c_str());

	const std::shared_ptr<Errors> errors = std::make_shared<Errors>();

  if (response.status_code() == web::http::status_codes::UnprocessableEntity) {
    response.content_ready().wait();
    web::json::value json_value = response.extract_json().get();

    const utility::string_t user_message =
        json_value[_XPLATSTR("userMessage")].as_string();
		errors->userMessage(json_value[_XPLATSTR("userMessage")].as_string());
    error.append(" ").append(utility::conversions::to_utf8string(user_message));

    ERROR_LOG(user_message.c_str());

    if (!json_value[_XPLATSTR("fieldErrors")].is_null()) {
      web::json::array field_errors =
          json_value[_XPLATSTR("fieldErrors")].as_array();
      for (auto field : field_errors) {
        const web::json::object field_error = field.as_object();
        const utility::string_t message =
            field_error.at(_XPLATSTR("message")).as_string();
        const utility::string_t fieldName =
            field_error.at(_XPLATSTR("fieldName")).as_string();
        
				ERROR_LOG(utility::string_t(_XPLATSTR("fieldName "))
                      .append(fieldName)
                      .append(_XPLATSTR(" "))
                      .append(message)
                      .c_str());
      }
    }
  }
	result_->errors(errors);
  throw std::runtime_error(error.c_str());
}

web::http::client::http_client_config
LicenseExtractor::make_client_config(const int64_t &attempt) {
  web::http::client::http_client_config config;
  config.set_validate_certificates(false);
  return config;
}

web::http::http_response LicenseExtractor::send_request() {
  const std::chrono::seconds time_try_connection_{attempt_};
  //???	std::chrono::duration_cast<std::chrono::seconds>(attempt_);
  if (!message_.is_valid())
    throw std::runtime_error("message for request is empty");

  std::shared_ptr<web::http::http_pipeline_stage> countStage =
      std::make_shared<stage_handler>();
  client_.add_handler(countStage);

  auto start = std::chrono::steady_clock::now();
  while (true) {
    try {
      return client_.request(request_).get();
      break;
    } catch (web::http::http_exception &ex) {
      ucout << ex.error_code().value() << std::endl; // error code = 12029
      if (std::chrono::steady_clock::now() > (start + time_try_connection_)) {
        ERROR_LOG(utility::conversions::to_string_t(ex.what()).c_str());
        std::throw_with_nested(std::runtime_error(ex.what()));
      }
    }
  }
}

web::json::value
LicenseExtractor::make_request_message(const Message message_) {
  web::json::value message;
  message[_XPLATSTR("unp")] = web::json::value::string(message_.get_unp());
  message[_XPLATSTR("request")] = web::json::value::string(message_.get_uid());
  message[_XPLATSTR("agentId")] =
      web::json::value::string(message_.get_agent());
  INFO_LOG(message.serialize().c_str());
  return message;
}
