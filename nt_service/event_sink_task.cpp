#include "event_sink_task.h"
#include "client_license.h"
#include "constants.h"
#include "license_checker.h"
#include "tracer.h"

#include "message_sender.h"
#include <codecvt>
#include <locale>

using namespace std;
using namespace utility;

EventSink_Task::EventSink_Task() : licenseChecker_(new LicenseChecker()) {
  //this->open();
}

int EventSink_Task::svc() {

  ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T EventSink_Task::svc() (%t) \n")));
  try {
    if (!licenseChecker_->is_license_file() ||
        !licenseChecker_->verify_license()) {
      const shared_ptr<LicenseExtractor> licenseExtractor_ =
          licenseChecker_->make_license_extractor(60);
      ACE_DEBUG(
          (LM_INFO,
           ACE_TEXT(
               "%T Get_License_Task: attempt to get a license... :(%t) \n")));
      INFO_LOG(TM("Attempt to get a license..."));
      const shared_ptr<Result> result = licenseExtractor_->processing_license();
      if (result->host_status()->id() == lic::lic_host_status::ACTIVE) {
        string_t license = result->host_license()->license();
        if (!license.empty()) {
          write_license(result->host_license());
        }
      }
    }
  } catch (const boost::process::process_error &err) {
    ACE_ERROR(
        (LM_DEBUG, ACE_TEXT("%T (%t):\t\tEventSink_Task: %s \n"), err.what()));
    std::string str(err.what());
    const string_t message =
        conversions::to_string_t(str.substr(0, str.find_first_of(":")));
    MESSAGE_SENDER::instance()->send(_XPLATSTR("0#Critical#") + message);
    ERROR_LOG(message.c_str());
    // stop service
    raise(SIGINT);
  } catch (const runtime_error &err) {
    const string_t message = conversions::to_string_t(std::string(err.what()));
    MESSAGE_SENDER::instance()->send(_XPLATSTR("0#Critical#") + message);
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
      MESSAGE_SENDER::instance()->send(_XPLATSTR("0#Critical#") + message);
      ACE_ERROR((LM_DEBUG, ACE_TEXT("%T (%t):\t\tEventSink_Task: kill task\n"),
                 err.what()));
      // stop service
      raise(SIGINT);
    }
  }
  return 0;
}

int EventSink_Task::write_license(const shared_ptr<HostLicense> &host_license) {
  if (!licenseChecker_->save_license_to_file(host_license->license())) {
    ERROR_LOG(TM("Error: don't save license to file"));
    ACE_ERROR((LM_DEBUG, ACE_TEXT("%T Get_License_Task: Error: don't save "
                                  "license to file :(%t) \n")));
    return -1;
  } else {
    char_t log_msg[50];
    size_t fmt_len = ACE_OS::sprintf(
        log_msg, _XPLATSTR("Save new license to file: month - %d, year - %d"),
        host_license->month(), host_license->year());
    INFO_LOG(log_msg);
  }
  return 0;
}