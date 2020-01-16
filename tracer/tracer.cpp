#include "tracer.h"

P7_EXPORT tBOOL __cdecl utils::Send(tUINT16 i_wTrace_ID,
                                    eP7Trace_Level i_dwLevel,
                                    hP7_Trace_Module i_hModule, tUINT16 i_wLine,
                                    const char *i_pFile,
                                    const char *i_pFunction,
                                    const tXCHAR *i_pFormat) {
  return LOGGER::instance()->write(
      i_dwLevel, i_pFormat, i_wLine, i_pFile, i_pFunction);
}
