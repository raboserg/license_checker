#include "event_sink_task.h"
#include "client_license.h"
#include "constants.h"
#include "license_checker.h"
#include "tracer.h"

#include <codecvt>
#include <locale>

using namespace std;
using namespace utility;

EventSink_Task::EventSink_Task() { this->open(); }

int EventSink_Task::svc() {

  const unique_ptr<LicenseChecker> licenseChecker_ =
      make_unique<LicenseChecker>();

  ACE_DEBUG((LM_DEBUG, ACE_TEXT("EventSink_Task::svc()\n")));

  shared_ptr<Result> result;
  shared_ptr<LicenseExtractor> licenseExtractor_;
  try {
    if (!licenseChecker_->is_license_file(_XPLATSTR("")) ||
        !licenseChecker_->verify_license()) {
      licenseExtractor_ = licenseChecker_->make_license_extractor(60);
      INFO_LOG(
          (TM("Connect to server...") + licenseExtractor_->get_uri().host())
              .c_str());
      result = licenseExtractor_->processing_license();

      if (result->host_status()->id() == lic::lic_host_status::ACTIVE) {
        string_t license = result->host_license()->license();
        if (!license.empty()) {
          INFO_LOG(TM("Save new license - ??? MONTH, YEAR ???"));
          licenseChecker_->save_license_to_file(license);
        }
      }
    }
  } catch (const boost::process::process_error &err) {
    ACE_ERROR(
        (LM_DEBUG, ACE_TEXT("%T (%t):\t\tEventSink_Task: %s \n"), err.what()));
    std::string str(err.what());
    const std::string code = str.substr(0, str.find_first_of(":"));
    ERROR_LOG(conversions::to_string_t(code).c_str());
    ERROR_LOG(TM("SERVICE SHUTDOWN"));
    // shutdown service
    raise(SIGINT);
  } catch (const runtime_error &err) {
    ACE_ERROR((LM_DEBUG, ACE_TEXT("%T (%t):\t\tEventSink_Task: kill task\n"),
               err.what()));
    ERROR_LOG(conversions::to_string_t(std::string(err.what())).c_str());
    ERROR_LOG(TM("SERVICE SHUTDOWN"));
    // shutdown service
    raise(SIGINT);
  } catch (web::http::http_exception &err) {
    ERROR_LOG(conversions::to_string_t(err.what()).c_str());
    ACE_ERROR((LM_DEBUG, ACE_TEXT("%T (%t):\t\tEventSink_Task: kill task\n"),
               err.what()));
    if (err.error_code().value() == lic::error_code::MIME_TYPES)
      ERROR_LOG(TM("SERVICE SHUTDOWN"));
    // shutdown service
    raise(SIGINT);
  }
  return 0;
}

typedef ACE_Unmanaged_Singleton<EventSink_Task, ACE_Null_Mutex>
    LICENSE_WORKER_TASK;
