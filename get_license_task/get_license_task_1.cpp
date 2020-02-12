#include "get_license_task.h"
#include "client_license.h"
#include "constants.h"
#include "tracer.h"

/// const uint64_t dsfds = utility::datetime::from_days(1); //???

const utility::string_t LIC =
    L"Mg==.MDAwMQ==.MjAyMC0xMi0wN1QxMzoxNjoyN1o=."
    L"txNi2dB9pSz3DAOi2Kq2zA62ym6lEx1iJcrSmK0urV0=."
    L"dXz+MH3Td1Pay3qZFbrWgybE3iith0PPaptDkNMHZsh/fpHdCvqwrtbzl68oeTKV";

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
  ACE_DEBUG((LM_INFO, ACE_TEXT("%T (%t):\t~Get_License_Task()\n")));
}

int Get_License_Task::open(ACE_Time_Value tv1) {
  reactor()->schedule_timer(this, 0, tv1, tv1);
  if (this->activate(THR_NEW_LWP, n_threads_) == -1)
    ACE_ERROR_RETURN(
        (LM_ERROR, ACE_TEXT("%T (%t):\tGet_License_Task: activate failed")),
        -1);
  else
    ACE_DEBUG((LM_INFO,
               ACE_TEXT("%T (%t):\tGet_License_Task: started %d threads\n"),
               n_threads_));
  return 0;
}

void Get_License_Task::close() {
  this->putq(new ACE_Message_Block(0, ACE_Message_Block::MB_STOP));
  reactor()->cancel_timer(this);
  ACE_DEBUG((LM_INFO, ACE_TEXT("%T (%t):\tGet_License_Task: cancel timer\n")));
}

int Get_License_Task::handle_timeout(const ACE_Time_Value &tv, const void *) {
  ACE_UNUSED_ARG(tv);
  ACE_DEBUG(
      (LM_DEBUG, ACE_TEXT("%T (%t):\tGet_License_Task::handle_timeout\n")));
  try {
    if (licenseChecker_->check_update_day()) {
      this->putq(new ACE_Message_Block(0, ACE_Message_Block::MB_SIG));
    } else {
      // SET TIMER FOR NEXT CHECK UPDATE DAY = 24 * 60 * 60
      schedule_wait(lic::constats::SECS_IN_DAY);
    }
  } catch (const std::runtime_error &err) {
    ACE_DEBUG(
        (LM_DEBUG, ACE_TEXT("%T (%t):\tGet_License_Task: \n"), err.what()));
    CRITICAL_LOG(utility::conversions::to_string_t(err.what()).c_str());
    this->putq(new ACE_Message_Block(0, ACE_Message_Block::MB_STOP));
  }
  return 0;
}

int Get_License_Task::handle_exception(ACE_HANDLE) {
  ACE_DEBUG(
      (LM_DEBUG, ACE_TEXT("%T (%t):\tGet_License_Task::handle_exception()\n")));
  this->putq(new ACE_Message_Block(0, ACE_Message_Block::MB_STOP));
  return -1;
}

int Get_License_Task::svc() {

  ACE_DEBUG((LM_INFO, ACE_TEXT("%T (%t):\tGet_License_Task: task started\n")));

  ACE_Message_Block *message = 0;

  for (;;) {

    if (this->getq(message) == -1)
      ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("getq")), -1);

    if (message->msg_type() == ACE_Message_Block::MB_STOP) {
      ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T (%t):\tGet_License_Task: MB_STOP\n")));
      message->release();
      break;
    }

    if (message->msg_type() == ACE_Message_Block::MB_SIG) {
      try {
        const std::shared_ptr<LicenseExtractor> licenseExtractor_ =
            licenseChecker_->make_license_extractor(1);

        // TRY GET LICENSE ?????
        // const utility::string_t license =
        // licenseExtractor_->receive_license();
        const utility::string_t license = licenseExtractor_->receive_license();

        if (license.empty()) {
          // SHEDULE TIME FOR NEXT TRY GET LICENSE
          schedule_wait(10);
        } else {
          const ACE_Date_Time date =
              licenseChecker_->extract_license_date(license);
          licenseChecker_->save_license_to_file(license);
        }
      } catch (const std::runtime_error &err) {
        reactor()->end_reactor_event_loop(); //???
        CRITICAL_LOG(utility::conversions::to_string_t(err.what()).c_str());
        ACE_ERROR_RETURN(
            (LM_ERROR, ACE_TEXT("%T (%t):\tGet_License_Task: killing task\n"),
             err.what()),
            -1);
      }
    }
  }
  ACE_DEBUG((LM_INFO, ACE_TEXT("%T (%t):\tGet_License_Task: task finished\n")));
  return 0;
}

int Get_License_Task::schedule_wait(const int seconds) {
  reactor()->cancel_timer(this);
  ACE_Time_Value tv1(seconds, 0);
  reactor()->schedule_timer(this, 0, tv1, tv1);
  return 0;
}
