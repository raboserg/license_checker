#ifndef TRACER_H
#define TRACER_H

#include <P7_Trace.h>
#include <Singleton.h>

#define LOGIN_CONNECT                                                          \
  TM("/P7.Sink=Baical /P7.Pool=32768 /P7.PSize=65536 /P7.Addr=127.0.0.1 "      \
     "/P7:Port=9009")

#define P7_DELIVER(i_wID, i_eLevel, i_hModule, ...)                            \
  Trace(i_wID, i_eLevel, i_hModule, (tUINT16)__LINE__, __FILE__, __FUNCTION__, \
        __VA_ARGS__)

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

  void trace(const tXCHAR *text, const char *file, const tUINT16 line,
             const char *fun) {
    write(EP7TRACE_LEVEL_TRACE, text, line, file, fun);
  }

  void error(const tXCHAR *text, const char *file, const tUINT16 line,
             const char *fun) {
    write(EP7TRACE_LEVEL_ERROR, text, line, file, fun);
  }

  void critical(const tXCHAR *text, const char *file, const tUINT16 line,
                const char *fun) {
    write(EP7TRACE_LEVEL_CRITICAL, text, line, file, fun);
  }

  void info(const tXCHAR *text, const char *file, const tUINT16 line,
            const char *fun) {
    write(EP7TRACE_LEVEL_INFO, text, line, file, fun);
  }

  void warning(const tXCHAR *text, const char *file, const tUINT16 line,
               const char *fun) {
    write(EP7TRACE_LEVEL_WARNING, text, line, file, fun);
  }

  void debug(const tXCHAR *text, const char *file, const tUINT16 line,
             const char *fun) {
    write(EP7TRACE_LEVEL_DEBUG, text, line, file, fun);
  }

  void write(const eP7Trace_Level level, const tXCHAR *text, const tUINT16 line,
             const char *file, const char *fun, ...) {
    if (tracer_) {
      tracer_->Trace(0, level, nullptr, line, file, fun, text);
    }
    // tracer_->P7_TRACE()
  }

private:
  IP7_Client *client_;
  IP7_Trace *tracer_;
  IP7_Trace::hModule module_;
};

typedef utils::Singleton<Logger> LOGGER;

#endif
