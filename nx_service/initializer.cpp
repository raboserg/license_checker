#include "initializer.h"
#include "callback.h"
#include "tracer.h"

Process::Process(void)
    : opt_install(0), opt_remove(0), opt_start(0), opt_kill(0), opt_type(0),
      opt_debug(0), opt_startup(0) {
  ACE_OS::strcpy(progname, "service");
  ACE::init();
}

Process::~Process(void) { ACE::fini(); }
void Process::print_usage_and_die(void) {
  ACE_DEBUG(
      (LM_INFO,
       "Usage: %s"
       " -in -r -s -k -tn -d\n"
       "  -i: Install this program as an NT service, with specified startup\n"
       "  -r: Remove this program from the Service Manager\n"
       "  -s: Start the service\n"
       "  -k: Kill the service\n"
       "  -t: Set startup for an existing service\n"
       "  -d: Debug; run as a regular application\n",
       progname, 0));
  ACE_OS::exit(1);
}

void Process::parse_args(int argc, char *argv[]) {
  ACE_Get_Opt get_opt(argc, argv, ACE_TEXT("i:rskt:d"));
  int c;

  while ((c = get_opt()) != -1)
    switch (c) {
    case 'i':
      opt_install = 1;
      opt_startup = ACE_OS::atoi(get_opt.opt_arg());
      if (opt_startup <= 0)
        print_usage_and_die();
      break;
    case 'r':
      opt_remove = 1;
      break;
    case 's':
      opt_start = 1;
      break;
    case 'k':
      opt_kill = 1;
      break;
    case 't':
      opt_type = 1;
      opt_startup = ACE_OS::atoi(get_opt.opt_arg());
      if (opt_startup <= 0)
        print_usage_and_die();
      break;
    case 'd':
      opt_debug = 1;
      break;
    default:
      // -i can also be given without a value - if so, it defaults
      // to defined value.
      if (ACE_OS::strcmp(get_opt.argv()[get_opt.opt_ind() - 1],
                         ACE_TEXT("-i")) == 0) {
        opt_install = 1;
        opt_startup = DEFAULT_SERVICE_INIT_STARTUP;
      } else {
        print_usage_and_die();
      }
      break;
    }
}

int Process::run(int argc, char *argv[]) {
  SERVICE::instance()->name(ACE_TEXT("itVPNAgent"), ACE_TEXT("itVPNAgent"));

  parse_args(argc, argv);

  if (opt_install && !opt_remove) {
    // svc_status_
    if (-1 == SERVICE::instance()->insert(opt_startup)) {
      ACE_ERROR((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("insert"),
                 ACE_TEXT("error code = %d, "), GetLastError()));
      return -1;
    }
    return 0;
  }

  if (opt_remove && !opt_install) {
    if (-1 == SERVICE::instance()->remove()) {
      ACE_ERROR((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("remove")));
      return -1;
    }
    return 0;
  }

  if (opt_start && opt_kill)
    print_usage_and_die();

  if (opt_start) {
    if (-1 == SERVICE::instance()->start_svc()) {
      ACE_ERROR((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("start")));
      return -1;
    }
    return 0;
  }

  if (opt_kill) {
    if (-1 == SERVICE::instance()->stop_svc()) {
      ACE_ERROR((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("stop")));
      return -1;
    }
    return 0;
  }

  if (opt_type) {
    if (-1 == SERVICE::instance()->startup(opt_startup)) {
      ACE_ERROR((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("set startup")));
      return -1;
    }
    return 0;
  }

  // If we get here, we either run the app in debug mode (-d) or are
  // being called from the service manager to start the service.
  if (opt_debug) {
    Callback *callback = new Callback;
    //???ACE_LOG_MSG->set_flags(ACE_Log_Msg::CUSTOM);
    ACE_LOG_MSG->priority_mask(
        LM_SHUTDOWN | LM_TRACE | LM_DEBUG | LM_INFO | LM_NOTICE | LM_WARNING |
            LM_STARTUP | LM_ERROR | LM_CRITICAL | LM_ALERT | LM_EMERGENCY,
        ACE_Log_Msg::PROCESS);
    ACE_LOG_MSG->set_flags(ACE_Log_Msg::MSG_CALLBACK);
    ACE_LOG_MSG->clr_flags(ACE_Log_Msg::OSTREAM);
    ACE_LOG_MSG->msg_callback(callback);
    //   std::ofstream *output_file = new std::ofstream("ntsvc.log", ios::out);
    // if (output_file && output_file->rdstate() == ios::goodbit) {
    //	ACE_LOG_MSG->msg_ostream(output_file, 1);
    //	ACE_LOG_MSG->set_flags(ACE_Log_Msg::OSTREAM);
    //}

    /*ACE_LOG_MSG->open(argv[0],
                  ACE_Log_Msg::STDERR | ACE_Log_Msg::SYSLOG ,
       ACE_TEXT("itVPNAgentSyslog"));*/

    SetConsoleCtrlHandler(&ConsoleHandler, 1);
    SERVICE::instance()->svc();
    SERVICE::close();
  } else {

    ACE_LOG_MSG->open(argv[0], ACE_Log_Msg::STDERR | ACE_Log_Msg::SYSLOG,
                      "itVPNAgentSyslog");

    /*static ofstream *output_file = new ofstream("ntsvc.log", ios::out);
    if (output_file && output_file->rdstate() == ios::goodbit)
            ACE_LOG_MSG->msg_ostream(output_file, 1);
    ACE_LOG_MSG->open(argv[0],
            ACE_Log_Msg::STDERR | ACE_Log_Msg::OSTREAM |
            ACE_Log_Msg::SYSLOG | ACE_Log_Msg::LOGGER,
            0);
    ACE_LOG_MSG->enable_debug_messages();*/

    ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T (%t): Starting service.\n")));
    DEBUG_LOG(TM("Starting service"));

    ACE_NT_SERVICE_RUN(itVPNAgent, SERVICE::instance(), ret);
    if (ret == 0) {
      DEBUG_LOG(TM("Couldn't start service"));
      ACE_ERROR(
          (LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("Couldn't start service")));
    } else
      ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T (%t): Service stopped.\n")));
  }

  return 0;
}
