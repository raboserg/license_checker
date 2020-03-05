#include "get_license_task.h"
#include "ace/Date_Time.h"
#include "ace/OS_NS_time.h"
#include "client_license.h"
#include "constants.h"
//#include "parser_ini.h"
#include "tracer.h"

#include "message_sender.h"

using namespace std;
using namespace utility;
// const uint64_t dsfds = utility::datetime::from_days(1);
// dsfds/10000/1000 = seconds

const char_t *module = _XPLATSTR("Get_License_Task");

Get_License_Task::Get_License_Task(const int &try_get_license_mins)
    : ACE_Task<ACE_MT_SYNCH>(ACE_Thread_Manager::instance()),
      licenseChecker_(new LicenseChecker()), day_counter_(0),
      try_get_license_mins_(try_get_license_mins) {
  this->reactor(ACE_Reactor::instance());
}

Get_License_Task::Get_License_Task(ACE_Thread_Manager *thr_mgr,
                                   const int &try_get_license_mins)
    : ACE_Task<ACE_MT_SYNCH>(thr_mgr),
      try_get_license_mins_(try_get_license_mins) {
  reactor(ACE_Reactor::instance());
}

Get_License_Task::~Get_License_Task() {
  reactor()->cancel_timer(this);
  ACE_DEBUG((LM_INFO, ACE_TEXT("%T ~Get_License_Task() :(%t) \n")));
}

int Get_License_Task::open(const ACE_Time_Value tv1) {
  timerId_ = reactor()->schedule_timer(this, 0, tv1, ACE_Time_Value::zero);
  return 0;
}

int Get_License_Task::close(u_long arg) { return 0; }

int Get_License_Task::handle_timeout(const ACE_Time_Value &current_time,
                                     const void *) {
  const time_t epoch = ((timespec_t)current_time).tv_sec;
  ACE_DEBUG((LM_INFO,
             ACE_TEXT("%T %I Get_License_Task: handle timeout :(%t) %s"),
             ACE_OS::ctime(&epoch)));
  if (activate(THR_NEW_LWP) == -1)
    ACE_ERROR_RETURN(
        (LM_ERROR, ACE_TEXT("%T Get_License_Task: activate failed :(%t) \n")),
        -1);
  return 0;
}

int Get_License_Task::handle_exception(ACE_HANDLE) {
  ACE_DEBUG(
      (LM_DEBUG, ACE_TEXT("%T Get_License_Task::handle_exception() :(%t) \n")));
  return -1;
}

int Get_License_Task::svc() {
  ACE_DEBUG((LM_INFO, ACE_TEXT("%T Get_License_Task: task started :(%t) \n")));
  shared_ptr<Result> result;
  try {
    if (licenseChecker_->is_license_update_day() &&
        licenseChecker_->is_license_file()) {
      ACE_DEBUG(
          (LM_INFO,
           ACE_TEXT(
               "%T Get_License_Task: attempt to get a license... :(%t) \n")));
      INFO_LOG(TM("Attempt to get a license..."));

      const shared_ptr<LicenseExtractor> licenseExtractor_ =
          licenseChecker_->make_license_extractor(1);
      result = licenseExtractor_->processing_license();

      if (result->host_status()->id() == lic::lic_host_status::ACTIVE) {
        string_t license = result->host_license()->license();
        if (license.empty()) {
          // SHEDULE TIME FOR NEXT TRY GET LICENSE
          //???schedule_handle_timeout(lic::constants::WAIT_NEXT_TRY_GET_SECS);
          schedule_handle_timeout(next_try_get_license_secs());
          ACE_DEBUG((
              LM_INFO,
              ACE_TEXT("%T Get_License_Task: Retrieved license "
                       "is empty, try to get after five minutes... :(%t) \n")));
          INFO_LOG(TM(
              "Retrieved license is empty, try to get after five minutes..."));
        } else {
          // read and check the date of current license
          const shared_ptr<HostLicense> host_license = result->host_license();
          if (host_license != nullptr && host_license->month() != 0) {
            if (licenseChecker_->is_check_licenses_months(
                    host_license->month())) {
              licenseChecker_->save_license_to_file(license);
              INFO_LOG(TM("Save new license to file- MONTH, YEAR ???"));
            }
            // schedule timer to check day for update : 24 * 60 * 60 =
            //!!!!!schedule_handle_timeout(lic::constants::NEXT_DAY_WAITING);
            schedule_handle_timeout(next_try_get_license_secs());
          } else {
            //?????????
            MESSAGE_SENDER::instance()->send(
                _XPLATSTR("1#Host License#Error restponse: retrived host "
                          "license is wrong"));
            schedule_handle_timeout(next_try_get_license_secs());
          }
        }
      } else {
        // TODO:save state to file ???
        // SHADULE TIME FOR NEXT GET LICENSE STATE - 5 minutes
        //???schedule_handle_timeout(lic::constants::WAIT_NEXT_TRY_GET_SECS);
        schedule_handle_timeout(next_try_get_license_secs());
      }
    } else {
      day_counter_++;
      // set timer for next check update day: 24 * 60 * 60
      //!!!!!schedule_handle_timeout(lic::constants::NEXT_DAY_WAITING);
      schedule_handle_timeout(next_try_get_license_secs());
      // TODO:save state to file ???
      INFO_LOG(TM("Wait next day"));
    }
  } catch (const boost::process::process_error &err) {
    std::string str(err.what());
    const string_t message =
        conversions::to_string_t(str.substr(0, str.find_first_of(":")));
    MESSAGE_SENDER::instance()->send(_XPLATSTR("0#Critical#") + message);
    ACE_ERROR(
        (LM_DEBUG, ACE_TEXT("%T Get_License_Task: %s :(%t) \n"), err.what()));
    ERROR_LOG(message.c_str());
    // shutdown service
    raise(SIGINT);
  } catch (const runtime_error &err) {
    const string_t message = conversions::to_string_t(std::string(err.what()));
    MESSAGE_SENDER::instance()->send(_XPLATSTR("0#Critical#") + message);
    ACE_ERROR((LM_DEBUG, ACE_TEXT("%T Get_License_Task: kill task :(%t) \n"),
               err.what()));
    ERROR_LOG(message.c_str());
    // shutdown service
    raise(SIGINT);
  } catch (web::http::http_exception &err) {
    const string_t message = conversions::to_string_t(err.what());
    ACE_ERROR((LM_DEBUG, ACE_TEXT("%T Get_License_Task: http error :(%t) \n"),
               err.what()));
    ERROR_LOG(message.c_str());
    if (err.error_code().value() == lic::error_code::MIME_TYPES) {
      MESSAGE_SENDER::instance()->send(_XPLATSTR("0#Critical#") + message);
      ACE_ERROR((LM_DEBUG, ACE_TEXT("%T Get_License_Task: kill task :(%t) \n"),
                 err.what()));
      // shutdown service
      raise(SIGINT);
    } else
      schedule_handle_timeout(next_try_get_license_secs());
  }
  ACE_DEBUG((LM_INFO, ACE_TEXT("%T Get_License_Task: task finished :(%t)\n")));
  return 0;
}

int Get_License_Task::schedule_handle_timeout(const int &seconds) {
  ACE_Time_Value tv1(seconds, 0);
  // reactor()->reset_timer_interval(this->timerId_, tv1);
  reactor()->cancel_timer(this);
  timerId_ = reactor()->schedule_timer(this, 0, tv1, ACE_Time_Value::zero);
  return 0;
}
