#include "event_sink_task.h"
#include "client_license.h"
#include "license_checker.h"
#include "constants.h"
#include "parser_ini.h"
#include "tracer.h"

EventSink_Task::EventSink_Task() { this->open(); }

int EventSink_Task::svc() {
	
	const std::unique_ptr<LicenseChecker> licenseChecker_ =
		std::make_unique<LicenseChecker>();

  for (ACE_Message_Block *log_blk; getq(log_blk) != -1;) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("EventSink_Task::svc()\n")));

    try {
      const web::http::uri address_ =
          PARSER::instance()->get_value(lic::config_keys::LICENSE_SRV_URI);

      if (!licenseChecker_->verify_license_file()) {
        
				// get unp
        const utility::string_t unp =
            PARSER::instance()->get_value(lic::config_keys::LICENSE_UNP);
        // get agent
        const utility::string_t agent =
            PARSER::instance()->get_value(lic::config_keys::LICENSE_AGENT_ID);
        // generate machine uid
        const utility::string_t uid = licenseChecker_->generate_machine_uid();

        const Message message_ = Message(uid, unp, agent);

        const std::unique_ptr<LicenseExtractor> licenseExtractor_ =
            std::make_unique<LicenseExtractor>(address_, message_, 5);

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
