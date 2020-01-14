#ifndef TRACER_H
#define TRACER_H

#include <P7_Trace.h>

struct Tracer {

  static void trace(const char *text) {
    IP7_Trace *l_iTrace = P7_Get_Shared_Trace(TM("LICENSE_CHECKER_TRC_LOG"));
    if (l_iTrace) {
      l_iTrace->P7_TRACE(0, TM(text), 0);
      l_iTrace->Release();
      l_iTrace = nullptr;
    }
  }

  static void error(const char *text) {
    IP7_Trace *l_iTrace = P7_Get_Shared_Trace(TM("LICENSE_CHECKER_TRC_LOG"));
    if (l_iTrace) {
      l_iTrace->P7_ERROR(0, TM(text), 0);
      l_iTrace->Release();
      l_iTrace = nullptr;
    }
  }

  static void critical(const char *text) {
    IP7_Trace *l_iTrace = P7_Get_Shared_Trace(TM("LICENSE_CHECKER_TRC_LOG"));
    if (l_iTrace) {
      l_iTrace->P7_CRITICAL(0, TM(text), 0);
      l_iTrace->Release();
      l_iTrace = nullptr;
    }
  }
};

#endif
