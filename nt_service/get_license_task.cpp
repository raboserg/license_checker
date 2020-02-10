#include "get_license_task.h"
#include "client_license.h"
#include "license_checker.h"
#include "tracer.h"

Get_License_Task::Get_License_Task()
    : ACE_Task<ACE_MT_SYNCH>(ACE_Thread_Manager::instance()), n_threads_(1) {
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

  try {
    // MOVE TO CONSTURCTOR
    const std::unique_ptr<LicenseChecker> licenseChecker_ =
        std::make_unique<LicenseChecker>();
    if (licenseChecker_->check_update_day()) {
      this->putq(new ACE_Message_Block(0, ACE_Message_Block::MB_SIG));
      ACE_DEBUG(
          (LM_DEBUG, ACE_TEXT("%T (%t):\tGet_License_Task::handle_timeout\n")));
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

      const std::unique_ptr<LicenseChecker> licenseChecker_ =
          std::make_unique<LicenseChecker>();

      /// const uint64_t dsfds = utility::datetime::from_days(1); //???

      // const utility::string_t lic =
      //    L"Mg==.MDAwMQ==.MjAyMC0xMi0wN1QxMzoxNjoyN1o=."
      //    L"txNi2dB9pSz3DAOi2Kq2zA62ym6lEx1iJcrSmK0urV0=."
      //    L"dXz+MH3Td1Pay3qZFbrWgybE3iith0PPaptDkNMHZsh/fpHdCvqwrtbzl68oeTKV";

      try {

        if (!licenseChecker_->verify_license_file()) {

          const std::shared_ptr<LicenseExtractor> licenseExtractor_ =
              licenseChecker_->make_license_extractor();

          const utility::string_t lic = licenseExtractor_->receive_license();

          if (!lic.empty()) {
            licenseChecker_->save_license_to_file(lic);
            ACE_Date_Time date = licenseChecker_->extract_license_date(lic);

            /*ACE_Time_Value tv1;
            reactor()->schedule_timer(this, 0, tv1, tv1);*/
          }
        } else {
          INFO_LOG(TM("License is SUCCESS")); //??????
        }
      } catch (const std::runtime_error &err) {
        ACE_DEBUG(
            (LM_DEBUG, ACE_TEXT("%T (%t):\tGet_License_Task: \n"), err.what()));
        CRITICAL_LOG(utility::conversions::to_string_t(err.what()).c_str());
        reactor()->end_reactor_event_loop(); //???
        break;
      }
    }
  }
  ACE_DEBUG((LM_INFO, ACE_TEXT("%T (%t):\tGet_License_Task: task finished\n")));
  return 0;
}
