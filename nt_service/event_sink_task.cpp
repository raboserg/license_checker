#include "event_sink_task.h"
#include "client_license.h"
#include "constants.h"
#include "license_checker.h"
#include "tracer.h"

using namespace std;
using namespace utility;

EventSink_Task::EventSink_Task() { this->open(); }

int EventSink_Task::svc() {

  //????raise(SIGINT);

  const unique_ptr<LicenseChecker> licenseChecker_ =
      make_unique<LicenseChecker>();

  // for (ACE_Message_Block *log_blk; getq(log_blk) != -1;) {

  ACE_DEBUG((LM_DEBUG, ACE_TEXT("EventSink_Task::svc()\n")));
  shared_ptr<Result> result;
  try {
    if (!licenseChecker_->is_license_file(_XPLATSTR("")) ||
        !licenseChecker_->verify_license()) {
      const shared_ptr<LicenseExtractor> licenseExtractor_ =
          licenseChecker_->make_license_extractor(60);

      result = licenseExtractor_->processing_license();
      if (result->host_status()->id() == lic::lic_host_status::ACTIVE) {
        string_t license = result->host_license()->license();
        if (!license.empty()) {
          INFO_LOG(TM("Save new license - MONTH, YEAR ???"));
          licenseChecker_->save_license_to_file(license);
        }
      }
    } else {
      INFO_LOG(TM("License is SUCCESS"));
    }

  } catch (const runtime_error &err) {
    ERROR_LOG(conversions::to_string_t(err.what()).c_str());

    raise(SIGINT);

  } catch (web::http::http_exception &ex) {
    ERROR_LOG(conversions::to_string_t(ex.what()).c_str());
    if (result->errors() == nullptr) {
      INFO_LOG(TM("Errors is nullptr"));
    } else {
      INFO_LOG((TM("Errors is not nullptr: ") + result->errors()->userMessage())
                   .c_str());
      ACE_ERROR_RETURN(
          (LM_ERROR, ACE_TEXT("%T (%t):\t\tGet_License_Task: kill task - %s\n"),
           result->errors()->userMessage().c_str()),
          -1);
    }
  }
  //}
  return 0;
}

typedef ACE_Unmanaged_Singleton<EventSink_Task, ACE_Null_Mutex>
    LICENSE_WORKER_TASK;
