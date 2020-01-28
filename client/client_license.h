// client_license.h
#pragma once
#ifndef CLIENT_LICENSE_H
#define CLIENT_LICENSE_H

#include <constants.h>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/asyncrt_utils.h>
#include <cpprest/details/basic_types.h>
#include <tracer.h>

class LicenseExtractor {
public:
  LicenseExtractor(const web::http::uri &address_,
                   const web::json::value message_, const int64_t &attempt);

  utility::string_t receive_license();

private:
  const int64_t attempt_;
  web::json::value message_;
	const web::http::uri address_;
	web::http::http_request request_;
	web::http::client::http_client client_;

  web::http::client::http_client_config
  make_client_config(const int64_t &attempt);
  //web::json::value make_request_message();
  web::http::http_response send_request();
  void processing_errors(const web::http::http_response &response);
};

#endif
