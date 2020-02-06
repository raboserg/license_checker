#include "event_sink_task.h"
#define BOOST_NO_CXX11_DECLTYPE
#include "client_license.h"
#include "license_helper.h"

EventSink_Task::EventSink_Task() {
	this->open();
}

int EventSink_Task::svc() {

  for (ACE_Message_Block *log_blk; getq(log_blk) != -1;) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("EventSink_Task::svc()\n")));

    const std::unique_ptr<LicenseChecker> licenseChecker_ =
        std::make_unique<LicenseChecker>();

    try {
      const web::http::uri address_ =
          PARSER::instance()->get_value(lic::config_keys::LICENSE_SRV_URI);

      if (!licenseChecker_->verify_license_file()) {
        const std::unique_ptr<LicenseExtractor> licenseExtractor_ =
            std::make_unique<LicenseExtractor>(
                address_, lic::license_helper::make_request_message(), 5);

        const utility::string_t lic = licenseExtractor_->receive_license();
        if (!lic.empty()) {
          licenseChecker_->save_license_to_file(lic);
        }

      } else {
        INFO_LOG(TM("License is SUCCESS"));
      }
    } catch (const std::runtime_error &err) {
      ERROR_LOG(utility::conversions::to_string_t(err.what()).c_str());
      // raise(SIGSEGV);
    } catch (web::http::http_exception &ex) {
      ERROR_LOG(utility::conversions::to_string_t(ex.what()).c_str());
      // raise(SIGSEGV);
    }
  }
  return 0;
}

typedef ACE_Unmanaged_Singleton<EventSink_Task, ACE_Null_Mutex>
    LICENSE_WORKER_TASK;
