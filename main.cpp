#ifdef _WIN32
#include "initializer.h"
#else
#include "nxsvc.h"
#endif
#include "callback.h"
#include <constants.h>
#include <tracer.h>

int main(int argc, char *argv[]) {
//???ACE_LOG_MSG->set_flags(ACE_Log_Msg::CUSTOM);
// ACE_LOG_MSG->priority_mask(
//    LM_SHUTDOWN | LM_TRACE | LM_DEBUG | LM_INFO | LM_NOTICE | LM_WARNING |
//        LM_STARTUP | LM_ERROR | LM_CRITICAL | LM_ALERT | LM_EMERGENCY,
//    ACE_Log_Msg::PROCESS);
// ACE_LOG_MSG->start_tracing();
// ACE_LOG_MSG->set_flags(ACE_Log_Msg::MSG_CALLBACK);
// ACE_LOG_MSG->clr_flags(ACE_Log_Msg::OSTREAM);
// ACE_LOG_MSG->msg_callback(new Callback);
//   std::ofstream *output_file = new std::ofstream("ntsvc.log", ios::out);
// if (output_file && output_file->rdstate() == ios::goodbit) {
//	ACE_LOG_MSG->msg_ostream(output_file, 1);
//	ACE_LOG_MSG->set_flags(ACE_Log_Msg::OSTREAM);
//}

/*ACE_LOG_MSG->open(argv[0],
                          ACE_Log_Msg::STDERR | ACE_Log_Msg::SYSLOG ,
   ACE_TEXT("itVPNAgentSyslog"));*/
#ifdef _WIN32
  return PROCESS::instance()->run(argc, argv);
// setlocale(LC_ALL, "ru_RU.UTF-8");
#else
  return SERVICE::instance()->run(argc, argv);
#endif

  return 0;
}
