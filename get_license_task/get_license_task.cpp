#include "get_license_task.h"
#include "ace/Date_Time.h"
#include "client_license.h"
#include "constants.h"
#include "parser_ini.h"
#include "tracer.h"

// dsfds/10000/1000 = seconds
// const uint64_t dsfds = utility::datetime::from_days(1);

// const utility::string_t LIC =
//    L"Mg==.MDAwMQ==.MjAyMC0xMi0wN1QxMzoxNjoyN1o=."
//    L"txNi2dB9pSz3DAOi2Kq2zA62ym6lEx1iJcrSmK0urV0=."
//    L"dXz+MH3Td1Pay3qZFbrWgybE3iith0PPaptDkNMHZsh/fpHdCvqwrtbzl68oeTKV";

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
  this->timerId_ = reactor()->schedule_timer(this, 0, tv1, tv1);
  return 0;
}

void Get_License_Task::close() {
  reactor()->cancel_timer(this);
  ACE_DEBUG(
      (LM_INFO, ACE_TEXT("%T (%t):\t\tGet_License_Task: cancel timer\n")));
}

int Get_License_Task::handle_timeout(const ACE_Time_Value &tv, const void *) {
  ACE_UNUSED_ARG(tv);
  ACE_DEBUG(
      (LM_DEBUG, ACE_TEXT("%T (%t):\t\tGet_License_Task: handle timeout\n")));
  if (this->activate(THR_NEW_LWP) == -1)
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
  try {
    if (licenseChecker_->check_update_day()) {
      const std::shared_ptr<LicenseExtractor> licenseExtractor_ =
          licenseChecker_->make_license_extractor(1);
      // TRY GET LICENSE ?????
      licenseExtractor_->processing_license();
      const Result result = licenseExtractor_->get_result();
      if (result.state == lic::host_states::ACTIVE) {
        const utility::string_t license = result.license;
        if (license.empty()) {
          // SHEDULE TIME FOR NEXT TRY GET LICENSE
          schedule_handle_timeout(lic::constants::WAIT_NEXT_TRY_GET_SECS);
        } else {
          const ACE_Date_Time license_date =
              licenseChecker_->extract_license_date(license);
          const ACE_Date_Time current_date;
          if (current_date.month() != license_date.month())
            licenseChecker_->save_license_to_file(license);
        }
      } else {
        // TODO:save state to file ???
        // SHADULE TIME FOR NEXT GET LICENSE STATE - 5 minutes
        schedule_handle_timeout(lic::constants::WAIT_NEXT_TRY_GET_SECS);
      }
    } else {
      // set timer for next check update day: 24 * 60 * 60
      schedule_handle_timeout(lic::constants::WAIT_NEXT_DAY_SECS);
      // TODO:save state to file ???
      INFO_LOG(TM("Wait next day"));
    }
  } catch (const std::runtime_error &err) {
    CRITICAL_LOG(utility::conversions::to_string_t(err.what()).c_str());
    shutdown_service(); //???
    ACE_ERROR_RETURN(
        (LM_ERROR, ACE_TEXT("%T (%t):\t\tGet_License_Task: kill task - %s\n"),
         err.what()),
        -1);
  }
  ACE_DEBUG(
      (LM_INFO, ACE_TEXT("%T (%t):\t\tGet_License_Task: task finished\n")));
  return 0;
}

int Get_License_Task::schedule_handle_timeout(const int &seconds) {
  ACE_Time_Value tv1(seconds, 0);
  // reactor()->cancel_timer(this);
  // reactor()->schedule_timer(this, 0, tv1, tv1);
  reactor()->reset_timer_interval(this->timerId_, tv1);
  return 0;
}
