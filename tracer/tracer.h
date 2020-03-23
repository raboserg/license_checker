#ifndef TRACER_H
#define TRACER_H

#include <P7_Trace.h>
#include <Singleton.h>
#include <cpprest/details/basic_types.h>

#include <functional>

#define LOGIN_CONNECT \
  TM("/P7.Sink=FileTxt /P7.Dir=D:/Logs/ /P7.Format=\" %lv [%tf] %ms\"")

template <typename T>
using tracer_unique_ptr = std::unique_ptr<T, std::function<void(T *)>>;

namespace itvpnagent {

class Logger {
 public:
  tBOOL write(const eP7Trace_Level level, const tXCHAR *text,
              const tUINT16 line, const char *file, const char *fun, ...);

  Logger();

 private:
  const tracer_unique_ptr<IP7_Client> client__;
  tracer_unique_ptr<IP7_Trace> tracer__;

  utility::string_t make_connect_config();
};

extern P7_EXPORT tBOOL __cdecl Send(tUINT16 i_wTrace_ID,
                                    eP7Trace_Level i_dwLevel,
                                    hP7_Trace_Module i_hModule, tUINT16 i_wLine,
                                    const char *i_pFile,
                                    const char *i_pFunction,
                                    const tXCHAR *i_pFormat);
}  // namespace itvpnagent

typedef itvpnagent::Singleton<itvpnagent::Logger> LOGGER;

#define DELIVER(level, X) \
  LOGGER::instance()->write(level, X, __LINE__, __FILE__, __FUNCTION__);

#define INFO_LOG(X) DELIVER(EP7TRACE_LEVEL_INFO, X)
#define TRACE_LOG(X) DELIVER(EP7TRACE_LEVEL_TRACE, X);
#define DEBUG_LOG(X) DELIVER(EP7TRACE_LEVEL_DEBUG, X);
#define ERROR_LOG(X) DELIVER(EP7TRACE_LEVEL_ERROR, X);
#define CRITICAL_LOG(X) DELIVER(EP7TRACE_LEVEL_CRITICAL, X);

#endif
