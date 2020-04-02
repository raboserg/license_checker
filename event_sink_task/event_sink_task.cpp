#include "event_sink_task.h"
#include "client_license.h"
#include "constants.h"
#include "license_checker.h"
#include "message_sender.h"
#include "tools.h"
#include "tracer.h"
#include <codecvt>
#include <locale>

using namespace std;
using namespace utility;

EventSink_Task::EventSink_Task() : licenseChecker_(new LicenseChecker()) {
  // this->open();
}

int EventSink_Task::svc() {

  ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T EventSink_Task::svc() (%t) \n")));
  try {
    if (!licenseChecker_->is_license_file() ||
        !licenseChecker_->verify_license()) {

      const shared_ptr<LicenseExtractor> licenseExtractor_ =
          licenseChecker_->make_license_extractor(60);
      ACE_DEBUG((
          LM_INFO,
          ACE_TEXT("%T EventSink_Task: attempt to get a license... :(%t) \n")));
      INFO_LOG(TM("Attempt to get a license..."));
      const shared_ptr<Result> result = licenseExtractor_->processing_license();
      itvpnagent::Net::send_message(_XPLATSTR("2#Host Status#") +
                                    result->host_status()->name());
      INFO_LOG((TM("Host Status: ") + result->host_status()->name()).c_str());
      if (result->host_status()->id() == lic::lic_host_status::ACTIVE) {
        string_t license = result->host_license()->license();
        if (!license.empty()) {
          INFO_LOG(TM("Received a license"));
          write_license(result->host_license());
        }
      } else {
        INFO_LOG(TM("Try to get license arter 5 minutes"));
      }
    }
  } catch (const boost::process::process_error &err) {
    ACE_ERROR(
        (LM_DEBUG, ACE_TEXT("%T (%t):\t\tEventSink_Task: %s \n"), err.what()));
    std::string str(err.what());
    const string_t message =
        conversions::to_string_t(str.substr(0, str.find_first_of(":")));
    itvpnagent::Net::send_message(_XPLATSTR("0#Critical#") + message);
    ERROR_LOG(message.c_str());
    // stop service
    raise(SIGINT);
  } catch (const runtime_error &err) {
    const string_t message = conversions::to_string_t(std::string(err.what()));
    itvpnagent::Net::send_message(_XPLATSTR("0#Critical#") + message);
    ACE_ERROR((LM_DEBUG, ACE_TEXT("%T (%t):\t\tEventSink_Task: kill task\n"),
               err.what()));
    ERROR_LOG(message.c_str());
    // stop service
    raise(SIGINT);
  } catch (web::http::http_exception &err) {
    const string_t message = conversions::to_string_t(err.what());
    ERROR_LOG(message.c_str());
    ACE_ERROR((LM_DEBUG, ACE_TEXT("%T (%t):\t\tEventSink_Task: kill task\n"),
               err.what()));
    if (err.error_code().value() == lic::error_code::MIME_TYPES) {
      itvpnagent::Net::send_message(_XPLATSTR("0#Critical#") + message);
    }
  }
  return 0;
}

int EventSink_Task::write_license(const shared_ptr<HostLicense> &host_license) {
  string_t license = host_license->license();
  if (!licenseChecker_->save_license_to_file(license)) {
    ERROR_LOG(TM("Error: don't save license to file"));
    ACE_ERROR((LM_DEBUG, ACE_TEXT("%T EventSink_Task: Error: don't save "
                                  "license to file :(%t) \n")));
    return -1;
  } else {
    std::array<char_t, 50> log_msg;
    ACE_OS::sprintf(
        log_msg.data(),
        _XPLATSTR("Save new license to file: month - %d, year - %d"),
        host_license->month(), host_license->year());
    INFO_LOG(log_msg.data());
  }
  return 0;
}
