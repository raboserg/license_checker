//#include "worker_task.h"
//!!!!!!!!!!!!!!#include <parser_ini.h>
//!!!!!!!!!!!!!!#include "client_license.h"
#include "worker_task.h"
#include <parser_ini.h>
#include "client_license.h"
#include "license_checker.h"


static web::json::value make_request_message() {
  const std::unique_ptr<Parser> parser_ = std::make_unique<Parser>();
  const std::unique_ptr<LicenseChecker> licenseChecker_ =
      std::make_unique<LicenseChecker>();
  // get unp
  const utility::string_t unp =
      parser_->get_value(lic::config_keys::LICENSE_UNP);
  // get mac
  const utility::string_t mac =
      parser_->get_value(lic::config_keys::LICENSE_AGENT_ID);
  // generate machine uid
  const utility::string_t uid = licenseChecker_->generate_machine_uid();
  ucout << uid << std::endl;
  INFO_LOG(uid.c_str());
  web::json::value message;
  message[U("unp")] = web::json::value::string(unp);
  message[U("request")] = web::json::value::string(uid);
  message[U("agentId")] = web::json::value::string(mac);
  return message;
}

int License_Worker_Task::svc() {
  for (ACE_Message_Block *log_blk; getq(log_blk) != -1;) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("License_Worker_Task::svc()\n")));
    const std::unique_ptr<LicenseChecker> licenseChecker_ =
        std::make_unique<LicenseChecker>();

    try {
      const web::http::uri address_ =
          PARSER::instance()->get_value(lic::config_keys::LICENSE_SRV_URI);

      const bool is_license_update = licenseChecker_->verify_license_file();
      if (is_license_update) {
        DEBUG_LOG(TM("Need to get machine MAC"));
        //////////////////////////////////////
        // Need to create client here
        //////////////////////////////////////
      } else {
        INFO_LOG(TM("License is SUCCESS"));
        // end thread
      }

      const std::unique_ptr<LicenseExtractor> licenseExtractor_ =
          std::make_unique<LicenseExtractor>(address_, make_request_message(),
                                             5);

      utility::string_t lic = licenseExtractor_->receive_license();
      /*if (!lic.empty()) {
        licenseChecker_->save_license_to_file(lic);
      }*/
    } catch (const std::runtime_error &err) {
      ERROR_LOG(utility::conversions::to_string_t(err.what()).c_str());
      //raise(SIGSEGV);
    } catch (web::http::http_exception &ex) {
      ERROR_LOG(utility::conversions::to_string_t(ex.what()).c_str());
      //raise(SIGSEGV);
    }
  }
  return 0;
}

typedef ACE_Unmanaged_Singleton<License_Worker_Task, ACE_Null_Mutex>
    LICENSE_WORKER_TASK;
