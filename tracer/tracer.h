#ifndef TRACER_H
#define TRACER_H
#include "tools.h"
#include <P7_Trace.h>
#include <Singleton.h>
#include <functional>

#define LOGIN_CONNECT                                                          \
  TM("/P7.Sink=FileTxt /P7.Dir=D:/Logs/ /P7.Format=\" %lv [%tf] %ms\"")

template <typename T>
using tracer_unique_ptr = std::unique_ptr<T, std::function<void(T *)>>;

namespace utils {
class Logger {
public:
  tBOOL write(const eP7Trace_Level level, const tXCHAR *text,
              const tUINT16 line, const char *file, const char *fun, ...) {
    if (tracer__) {
      tracer__->Trace(0, level, nullptr, line, file, fun, text);
      return TRUE;
    }
    return FALSE;
  }

  Logger()
      : client__(P7_Create_Client(make_connect_config().c_str()),
                 [](IP7_Client *client) { client->Release(); }),
        tracer__(P7_Create_Trace(client__.get(), TM("TraceChannel")),
                 [](IP7_Trace *tracer) { tracer->Release(); }) {}

  /*Logger() {
          const utility::string_t sdfdsfd = make_connect_config();
    client__ = {P7_Create_Client(TM("/p7.sink=filetxt /p7.dir=D:/logs/
  /p7.format=\" %lv [%tf] %ms\"")),
                [](IP7_Client *client) { client->Release(); }};
    tracer__ = {P7_Create_Trace(client__.get(), TM("TraceChannel")),
                [](IP7_Trace *tracer) { tracer->Release(); }};
  }*/

private:
  tracer_unique_ptr<IP7_Client> client__;
  tracer_unique_ptr<IP7_Trace> tracer__;

  utility::string_t make_connect_config() {
    //#ifdef CONNEC_CONFIG
    return TM("/P7.Sink=Baical /P7.Pool=32768 /P7.PSize=65536 "
              "/P7.Addr=127.0.0.1 /P7:Port=9009");
    //#else
    //    const utility::string_t dir =
    //        TM("/P7.Dir=") + lic::current_module_path() + TM("logs");
    //    utility::string_t connect(TM("/P7.Sink=FileTxt ") + dir +
    //                              TM(" /P7.Format=\" %lv [%tf] %ms\""));
    //    return connect;
    //#endif
  }
};

extern P7_EXPORT tBOOL __cdecl Send(tUINT16 i_wTrace_ID,
                                    eP7Trace_Level i_dwLevel,
                                    hP7_Trace_Module i_hModule, tUINT16 i_wLine,
                                    const char *i_pFile,
                                    const char *i_pFunction,
                                    const tXCHAR *i_pFormat);
} // namespace utils

typedef utils::Singleton<utils::Logger> LOGGER;

#define DELIVER(level, X)                                                      \
  LOGGER::instance()->write(level, X, __LINE__, __FILE__, __FUNCTION__);

#define INFO_LOG(X) DELIVER(EP7TRACE_LEVEL_INFO, X)
#define TRACE_LOG(X) DELIVER(EP7TRACE_LEVEL_TRACE, X);
#define DEBUG_LOG(X) DELIVER(EP7TRACE_LEVEL_DEBUG, X);
#define ERROR_LOG(X) DELIVER(EP7TRACE_LEVEL_ERROR, X);
#define CRITICAL_LOG(X) DELIVER(EP7TRACE_LEVEL_CRITICAL, X);

#endif
