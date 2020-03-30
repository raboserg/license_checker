#include "tracer.h"
#include "parser_ini.h"
#include "tools.h"

static const long s_is_debug = is_debug;
static const char s_connect_type[] = log_config;

namespace itvpnagent {

static const utility::char_t s_log_text_format[] = {
    TM("/P7.Format=\"%lv [%tf] %ms\"")};

static utility::string_t log_file_config = {
    TM("/P7.Sink=FileTxt /P7.Format=\"%lv [%tf] %ms\" /P7.Dir=")};

static const utility::string_t lig_server_config =
    TM("/P7.Sink=Baical /P7.Pool=32768 /P7.PSize=65536 /P7.Addr=127.0.0.1 "
       "/P7:Port=9009");

Logger::Logger()
    : client__(P7_Create_Client(make_connect_config().c_str()),
               [](IP7_Client *client) { client->Release(); }),
      tracer__(P7_Create_Trace(client__.get(), TM("TraceChannel")),
               [](IP7_Trace *tracer) { tracer->Release(); }) {}

Logger::Logger(const utility::string_t &log_path)
    : client__(P7_Create_Client(make_connect_config(log_path).c_str()),
               [](IP7_Client *client) { client->Release(); }),
      tracer__(P7_Create_Trace(client__.get(), TM("TraceChannel")),
               [](IP7_Trace *tracer) { tracer->Release(); }) {}

tBOOL Logger::write(const eP7Trace_Level level, const tXCHAR *text,
                    const tUINT16 line, const char *file, const char *fun,
                    ...) {
  if (tracer__) {
    tracer__->Trace(0, level, nullptr, line, file, fun, text);
    return TRUE;
  }
  return FALSE;
}

utility::string_t Logger::make_connect_config() {
  if (is_debug)
    return lig_server_config;
  else {
#ifdef _WIN32
    log_file_config.append(System::current_module_path().append(TM("logs")));
#else
    // return PARSER::instance()->options().log_files_path;
    log_file_config.append(System::current_module_path().append(TM("logs")));
#endif
    return log_file_config;
  }
}

utility::string_t
Logger::make_connect_config(const utility::string_t &log_path) {
  if (is_debug)
    return lig_server_config;
  else {
#ifdef _WIN32
    log_file_config.append(log_path);
#else
    log_file_config.append(log_path);
#endif
    return log_file_config;
  }
}

P7_EXPORT tBOOL __cdecl Send(tUINT16 i_wTrace_ID, eP7Trace_Level i_dwLevel,
                             hP7_Trace_Module i_hModule, tUINT16 i_wLine,
                             const char *i_pFile, const char *i_pFunction,
                             const tXCHAR *i_pFormat) {
  return LOGGER::instance()->write(i_dwLevel, i_pFormat, i_wLine, i_pFile,
                                   i_pFunction);
}
} // namespace itvpnagent
