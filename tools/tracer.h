#ifndef TRACER_H
#define TRACER_H

#include <P7_Trace.h>

#define LOGIN_CONNECT                                                          \
  TM("/P7.Sink=Baical /P7.Pool=32768 /P7.PSize=65536 /P7.Addr=127.0.0.1 "      \
     "/P7:Port=9009")

struct Tracer {

  static void trace(const tXCHAR *text, const tUINT16 line, const char *file,
                    const char *fun) {
    write(EP7TRACE_LEVEL_TRACE, text, line, file, fun);
  }

  static void error(const tXCHAR *text, const tUINT16 line, const char *file,
                    const char *fun) {
		write(EP7TRACE_LEVEL_ERROR, text, line, file, fun);

  }

  static void critical(const tXCHAR *text, const tUINT16 line, const char *file,
                       const char *fun) {
    write(EP7TRACE_LEVEL_CRITICAL, text, line, file, fun);
  }

  static void info(const tXCHAR *text, const tUINT16 line, const char *file,
                   const char *fun) {
    write(EP7TRACE_LEVEL_INFO, text, line, file, fun);
  }

  static void warning(const tXCHAR *text, const tUINT16 line, const char *file,
                      const char *fun) {
    write(EP7TRACE_LEVEL_WARNING, text, line, file, fun);
  }

  static void debug(const tXCHAR *text, const tUINT16 line, const char *file,
                    const char *fun) {
    write(EP7TRACE_LEVEL_DEBUG, text, line, file, fun);
  }

	static void write(const eP7Trace_Level level, const tXCHAR *text, const tUINT16 line,
		const char *file, const char *fun, ...) {
		IP7_Trace *lTrace_ = P7_Get_Shared_Trace(TM("LICENSE_CHECKER_TRC_LOG"));
		if (lTrace_) {
			lTrace_->Trace(0, level, 0, line, file, fun, text);
			lTrace_->Release();
			lTrace_ = nullptr;
		}
	}

	static void init() {
		if (Client_ == nullptr && Tracer_ == nullptr) {
			Client_ = P7_Create_Client(LOGIN_CONNECT);
			P7_Client_Share(Client_, TM("LICENSE_CHECKER_CLN_LOG"));
			Tracer_ = P7_Create_Trace(Client_, TM("TraceChannel"));
			Tracer_->Register_Thread(TM("Application"), 0);
			IP7_Trace::hModule l_hModule = nullptr;
			Tracer_->Register_Module(TM("Main"), &l_hModule);
			P7_Trace_Share(Tracer_, TM("LICENSE_CHECKER_TRC_LOG"));
		}
	}

	static void clear() {
		if (Client_ != nullptr) {
			Client_->Release();
			Client_ = nullptr;
		}
		if(Tracer_ == nullptr) {
			Tracer_->Release();
			Tracer_ = nullptr;
		}
	}

	static IP7_Client *Client_;
	static IP7_Trace *Tracer_;
};

#endif
