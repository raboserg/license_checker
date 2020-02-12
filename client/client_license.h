// client_license.h
#pragma once
#ifndef CLIENT_LICENSE_H
#define CLIENT_LICENSE_H

#define _TURN_OFF_PLATFORM_STRING

#include <cpprest/details/basic_types.h>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include "constants.h"

struct Result {
	int state;
	utility::string_t lic;
	utility::string_t year_lic;
	utility::string_t month_lic;
	utility::string_t errors; //???
};

class Message {
public:
  Message(const utility::string_t uid, const utility::string_t unp,
          const utility::string_t agent)
      : uid_(uid), unp_(unp), agent_(agent) {}

  utility::string_t get_uid() const { return uid_; }

  utility::string_t get_unp() const { return unp_; }

  utility::string_t get_agent() const { return agent_; }

	bool is_valid() {
		return !uid_.empty() && !unp_.empty() && !agent_.empty();
	}
private:
  const utility::string_t uid_;
  const utility::string_t unp_;
  const utility::string_t agent_;
};

class LicenseExtractor {
public:
  LicenseExtractor(const web::http::uri &address_, const Message &message_,
                   const int64_t &attempt);

  utility::string_t receive_license();

private:
	Result result_;
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
