#include "get_license_task.h"
#include "ace/Date_Time.h"
#include "ace/OS_NS_time.h"
#include "client_license.h"
#include "constants.h"
#include "parser_ini.h"
#include "tracer.h"

using namespace std;
using namespace utility;
// const uint64_t dsfds = utility::datetime::from_days(1);
// dsfds/10000/1000 = seconds

Get_License_Task::Get_License_Task()
    : ACE_Task<ACE_MT_SYNCH>(ACE_Thread_Manager::instance()), n_threads_(1),
      licenseChecker_(new LicenseChecker()) {
  this->reactor(ACE_Reactor::instance());
}

Get_License_Task::Get_License_Task(ACE_Thread_Manager *thr_mgr,
                                   const int n_threads)
    : ACE_Task<ACE_MT_SYNCH>(thr_mgr), n_threads_(n_threads) {
  reactor(ACE_Reactor::instance());
}

Get_License_Task::~Get_License_Task() {
  close();
  ACE_DEBUG((LM_INFO, ACE_TEXT("%T (%t):\t\t~Get_License_Task()\n")));
}

int Get_License_Task::open(ACE_Time_Value tv1) {
  this->timerId_ =
      reactor()->schedule_timer(this, 0, tv1, ACE_Time_Value::zero);
  return 0;
}

void Get_License_Task::close() {
  reactor()->cancel_timer(this);
  ACE_DEBUG(
      (LM_INFO, ACE_TEXT("%T (%t):\t\tGet_License_Task: cancel timer\n")));
}

int Get_License_Task::handle_timeout(const ACE_Time_Value &current_time,
                                     const void *) {
  time_t epoch = ((timespec_t)current_time).tv_sec;
  ACE_DEBUG((LM_INFO,
             ACE_TEXT("%T (%t):\t\tGet_License_Task: handle timeout: %s\n"),
             ACE_OS::ctime(&epoch)));
  if (activate(THR_NEW_LWP) == -1)
    ACE_ERROR_RETURN(
        (LM_ERROR, ACE_TEXT("%T (%t):\t\tGet_License_Task: activate failed")),
        -1);
  return 0;
}

int Get_License_Task::handle_exception(ACE_HANDLE) {
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("%T (%t):\t\tGet_License_Task::handle_exception()\n")));
  return -1;
}

int Get_License_Task::svc() {
  ACE_DEBUG(
      (LM_INFO, ACE_TEXT("%T (%t):\t\tGet_License_Task: task started\n")));
  shared_ptr<Result> result;
  try {
    if (licenseChecker_->is_license_update_day()) {
      const shared_ptr<LicenseExtractor> licenseExtractor_ =
          licenseChecker_->make_license_extractor(1);
      INFO_LOG(TM("Try to get a license"));
      result = licenseExtractor_->processing_license();

      if (result->host_status()->id() == lic::lic_host_status::ACTIVE) {
        string_t license = result->host_license()->license();
        if (license.empty()) {
          // SHEDULE TIME FOR NEXT TRY GET LICENSE
          schedule_handle_timeout(lic::constants::WAIT_NEXT_TRY_GET_SECS);
        } else {

          const ACE_Date_Time current_license_date =
              licenseChecker_->current_license_date();

          if (result->host_license()->month() != 0 &&
              current_license_date.month() != 0)
            if (result->host_license()->month() > current_license_date.month())
              licenseChecker_->save_license_to_file(license);

          INFO_LOG(TM("Save new license - MONTH, YEAR ???"));
          // schedule timer to check day for update : 24 * 60 * 60 =
          // WAIT_NEXT_DAY_SECS
          schedule_handle_timeout(lic::constants::WAIT_NEXT_TRY_GET_SECS);
        }
      } else {
        // TODO:save state to file ???
        // SHADULE TIME FOR NEXT GET LICENSE STATE - 5 minutes
        schedule_handle_timeout(lic::constants::WAIT_NEXT_TRY_GET_SECS);
      }
    } else {
      // set timer for next check update day: 24 * 60 * 60
      schedule_handle_timeout(lic::constants::NEXT_DAY_WAITING);
      // TODO:save state to file ???
      INFO_LOG(TM("Wait next day"));
    }
  } catch (const runtime_error &err) {
    CRITICAL_LOG(conversions::to_string_t(err.what()).c_str());
    shutdown_service(); //???
    ACE_ERROR_RETURN(
        (LM_ERROR, ACE_TEXT("%T (%t):\t\tGet_License_Task: kill task - %s\n"),
         err.what()),
        -1);

    if (result->errors() != nullptr) {
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
  ACE_DEBUG(
      (LM_INFO, ACE_TEXT("%T (%t):\t\tGet_License_Task: task finished\n")));
  return 0;
}

int Get_License_Task::schedule_handle_timeout(const int &seconds) {
  ACE_Time_Value tv1(seconds, 0);
  reactor()->reset_timer_interval(this->timerId_, tv1);
  return 0;
}
