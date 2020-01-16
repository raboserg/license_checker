#ifndef TRACER_H
#define TRACER_H

#include <P7_Trace.h>
#include <Singleton.h>

/*
BOOST_PROPERTY_TREE_DETAIL_PTREE_IMPLEMENTATION
# define BOOST_THROW_EXCEPTION(x)
::boost::exception_detail::throw_exception_(x,BOOST_THROW_EXCEPTION_CURRENT_FUNCTION,__FILE__,__LINE__)

template <class E>
void throw_exception_(E const & x, char const * current_function, char const *
file, int line)
{
        boost::throw_exception(
                set_info(
                        set_info(
                                set_info(
                                        enable_error_info(x),
                                        throw_function(current_function)),
                                throw_file(file)),
                        throw_line(line)));
}
*/

#define LOGIN_CONNECT                                                          \
  TM("/P7.Sink=Baical /P7.Pool=32768 /P7.PSize=65536 /P7.Addr=127.0.0.1 "      \
     "/P7:Port=9009")

namespace utils {
class Logger {
public:
  Logger() {
    client_ = P7_Create_Client(LOGIN_CONNECT);
    tracer_ = P7_Create_Trace(client_, TM("TraceChannel"));
    tracer_->Register_Thread(TM("App"), 0);
    tracer_->Register_Module(TM("Lisence check"), &module_);
  }

  ~Logger() {
    if (client_) {
      client_->Release();
      client_ = nullptr;
    }
    if (tracer_) {
      tracer_->Release();
      tracer_ = nullptr;
    }
  }

  tBOOL write(const eP7Trace_Level level, const tXCHAR *text,
              const tUINT16 line, const char *file, const char *fun, ...) {
    if (tracer_) {
      tracer_->Trace(0, level, nullptr, line, file, fun, text);
      return TRUE;
    }
    return FALSE;
  }

private:
  IP7_Client *client_;
  IP7_Trace *tracer_;
  IP7_Trace::hModule module_;
};

extern P7_EXPORT tBOOL __cdecl Send(tUINT16 i_wTrace_ID, eP7Trace_Level i_dwLevel,
	hP7_Trace_Module i_hModule, tUINT16 i_wLine,
	const char *i_pFile, const char *i_pFunction,
	const tXCHAR *i_pFormat);
////////////////////////////////////////////////////////////////////////////////
#define DELIVER(i_wID, i_eLevel, i_hModule, i_pFormat, ...)                    \
  utils::Send(i_wID, i_eLevel, i_hModule, (tUINT16)__LINE__, __FILE__,        \
               __FUNCTION__, i_pFormat)
////////////////////////////////////////////////////////////////////////////////
#define QTRACE(i_wID, i_hModule, i_pFormat)                                    \
  DELIVER(i_wID, EP7TRACE_LEVEL_TRACE, i_hModule, i_pFormat)

#define TRACE(i_hModule, i_pFormat) QTRACE(0, i_hModule, i_pFormat)
////////////////////////////////////////////////////////////////////////////////
#define QDEBUG(i_wID, i_hModule, i_pFormat)                                    \
  DELIVER(i_wID, EP7TRACE_LEVEL_DEBUG, i_hModule, i_pFormat)

#define DEBUGS(i_hModule, i_pFormat) QDEBUG(0, i_hModule, i_pFormat)
////////////////////////////////////////////////////////////////////////////////
#define QINFO(i_wID, i_hModule, i_pFormat)                                     \
  DELIVER(i_wID, EP7TRACE_LEVEL_INFO, i_hModule, i_pFormat)

#define INFO(i_hModule, i_pFormat) QINFO(0, i_hModule, i_pFormat)
////////////////////////////////////////////////////////////////////////////////
#define QWARNING(i_wID, i_hModule, i_pFormat)                                  \
  DELIVER(i_wID, EP7TRACE_LEVEL_WARNING, i_hModule, i_pFormat)

#define WARNING(i_hModule, i_pFormat) QWARNING(0, i_hModule, i_pFormat)
////////////////////////////////////////////////////////////////////////////////
#define QERROR(i_wID, i_hModule, i_pFormat)                                    \
  DELIVER(i_wID, EP7TRACE_LEVEL_ERROR, i_hModule, i_pFormat)

#define ERRORS(i_hModule, i_pFormat) QERROR(0, i_hModule, i_pFormat)
////////////////////////////////////////////////////////////////////////////////
#define QCRITICAL(i_wID, i_hModule, i_pFormat)                                 \
  DELIVER(i_wID, EP7TRACE_LEVEL_CRITICAL, i_hModule, i_pFormat)

#define CRITICAL(i_hModule, i_pFormat) QCRITICAL(0, i_hModule, i_pFormat)
////////////////////////////////////////////////////////////////////////////////
} // namespace utils

typedef utils::Singleton<utils::Logger> LOGGER;
#endif
