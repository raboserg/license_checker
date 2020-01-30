#include "client_license.h"

LicenseExtractor::LicenseExtractor(const web::http::uri &address,
                                   const web::json::value message,
                                   const int64_t &attempt)
    : client_(address, make_client_config(attempt)), message_(message),
      attempt_(attempt) {
  request_.set_method(web::http::methods::POST);
  request_.set_body(message_.serialize(),
                    web::http::details::mime_types::application_json);
}

utility::string_t LicenseExtractor::receive_license() {
  const web::http::http_response response = send_request();
  ucout << response.to_string() << std::endl;
  if (response.status_code() == web::http::status_codes::OK) {
    response.content_ready().wait();
		web::http::http_headers headers= response.headers();
		const utility::string_t content_type = headers[web::http::header_names::content_type];
		//text/html;charset=UTF-8
		web::json::value json_value = response.extract_json().get();
    if (!json_value[U("hostStatus")].is_null()) {
      const web::json::object host_status =
          json_value[U("hostStatus")].as_object();
      const int host_state = host_status.at(U("id")).as_integer();
      const utility::string_t host_state_name =
          host_status.at(U("name")).as_string();
      if (lic::host_states::ACTIVE == host_state) {
        TRACE_LOG(host_state_name.c_str());
        web::json::array licenses = json_value[U("hostLicenses")].as_array();
        utility::string_t license;
        const size_t size = licenses.size();
        if (!(size == 0)) {
          web::json::value license_item = licenses[licenses.size() - 1];
          license = license_item[U("license")].as_string();
          const utility::string_t license_exp_date =
              license_item[U("licenseExpirationDate")].as_string();
          utility::string_t license_msg(U("data: ") + license_exp_date +
                                        U("; lic: ") + license);
          TRACE_LOG(license_msg.c_str());
        }
        return license;
      }
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

  if (response.status_code() == web::http::status_codes::UnprocessableEntity) {
    response.content_ready().wait();
    web::json::value json_value = response.extract_json().get();

    const utility::string_t user_message =
        json_value[U("userMessage")].as_string();

    error.append(" ").append(utility::conversions::to_utf8string(user_message));

    ERROR_LOG(user_message.c_str());

    if (!json_value[U("fieldErrors")].is_null()) {
      web::json::array field_errors = json_value[U("fieldErrors")].as_array();
      for (auto field : field_errors) {
        const web::json::object field_error = field.as_object();
        const utility::string_t message =
            field_error.at(U("message")).as_string();
        const utility::string_t fieldName =
            field_error.at(U("fieldName")).as_string();
        ERROR_LOG(utility::string_t(U("fieldName "))
                      .append(fieldName)
                      .append(U(" "))
                      .append(message)
                      .c_str());
      }
    }
  }
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
  if (message_.is_null())
    throw std::runtime_error("message for request is empty");

  TRACE_LOG(message_.serialize().c_str());

  auto start = std::chrono::steady_clock::now();
  for (;;) {
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

// web::json::value LicenseExtractor::make_request_message() {
//  const std::unique_ptr<Parser> parser_ =
//      std::make_unique<Parser>(LIC_INI_FILE);
//  const std::unique_ptr<LicenseChecker> licenseChecker_ =
//      std::make_unique<LicenseChecker>();
//  // get unp
//  const utility::string_t unp =
//      parser_->get_value(lic::config_keys::LICENSE_UNP);
//  // get mac
//  const utility::string_t mac =
//      parser_->get_value(lic::config_keys::LICENSE_MAC);
//  // generate machine uid
//  const utility::string_t uid = licenseChecker_->generate_machine_uid();
//  INFO_LOG(uid.c_str());
//  web::json::value message;
//  message[U("unp")] = web::json::value::string(unp);
//  message[U("request")] = web::json::value::string(U("request"));
//  message[U("mac")] = web::json::value::string(mac);
//  return message;
//}
