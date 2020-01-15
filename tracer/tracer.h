#ifndef TRACER_H
#define TRACER_H

#include <P7_Trace.h>

#define LOGIN_CONNECT                                                          \
  TM("/P7.Sink=Baical /P7.Pool=32768 /P7.PSize=65536 /P7.Addr=127.0.0.1 "      \
     "/P7:Port=9009")

struct Tracer {

  /*tUINT16            i_wTrace_ID,
        eP7Trace_Level     i_eLevel,
        IP7_Trace::hModule i_hModule,
        tUINT16            i_wLine,
        const char        *i_pFile,
        const char        *i_pFunction,
        const tXCHAR      *i_pFormat,*/

  /*LEVEL i_hModule, 0  (tUINT16)__LINE__, __FILE__, __FUNCTION__, __VA_ARGS__*/

  // const tXCHAR      *i_pFormat,  tUINT16  i_wLine,	const char *i_pFile,
  // const char *i_pFunction,

	
  static void trace(const tXCHAR *text, tUINT16 line, const char *file,
                    const char *fun) {
    write(EP7TRACE_LEVEL_TRACE, text, line, file, fun);
  }

  static void error(const tXCHAR *text, tUINT16 line, const char *file,
                    const char *fun) {
    write(EP7TRACE_LEVEL_ERROR, text, line, file, fun);
  }

  static void critical(const tXCHAR *text, tUINT16 line, const char *file,
                       const char *fun) {
    write(EP7TRACE_LEVEL_CRITICAL, text, line, file, fun);
  }

  static void info(const tXCHAR *text, tUINT16 line, const char *file,
                   const char *fun) {
    write(EP7TRACE_LEVEL_INFO, text, line, file, fun);
  }

  static void warning(const tXCHAR *text, tUINT16 line, const char *file,
                      const char *fun) {
    write(EP7TRACE_LEVEL_WARNING, text, line, file, fun);
  }

  static void debug(const tXCHAR *text, tUINT16 line, const char *file,
                    const char *fun) {
    write(EP7TRACE_LEVEL_DEBUG, text, line, file, fun);
  }

	Tracer() {
		l_iClient = P7_Create_Client(LOGIN_CONNECT);
		P7_Client_Share(l_iClient, TM("LICENSE_CHECKER_CLN_LOG"));
		l_iTrace = P7_Create_Trace(l_iClient, TM("TraceChannel"));
		l_iTrace->Register_Thread(TM("Application"), 0);
		IP7_Trace::hModule l_hModule = nullptr;
		l_iTrace->Register_Module(TM("Main"), &l_hModule);
		P7_Trace_Share(l_iTrace, TM("LICENSE_CHECKER_TRC_LOG"));
	}

private:
	static IP7_Client *l_iClient;
	static IP7_Trace *l_iTrace;
	static IP7_Trace::hModule l_hModule;

	static void write(eP7Trace_Level level, const tXCHAR *text, tUINT16 line,
		const char *file, const char *fun, ...) {
		IP7_Trace *l_iTrace = P7_Get_Shared_Trace(TM("LICENSE_CHECKER_TRC_LOG"));
		if (l_iTrace) {
			l_iTrace->Trace(0, level, 0, line, file, fun, text);
			l_iTrace->Release();
			l_iTrace = nullptr;
		}
	}
};

#endif
