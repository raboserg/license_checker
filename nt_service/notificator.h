#pragma once

#include "eventsink.h"
#include <ace/Auto_Event.h>
#include <strsafe.h>
#include <tuple>

static const _bstr_t applicationName = "openvpn.exe";

static const _bstr_t wql_query_ =
    "SELECT * "
    "FROM __InstanceCreationEvent WITHIN 1 "
    "WHERE TargetInstance ISA 'Win32_Process' and "
    "TargetInstance.Name = "
    "'openvpn.exe'";

namespace WinNT {

class Notificator {

public:
  Notificator();
  Notificator(const _bstr_t name);
  void set_cencel(const bool flag);
  virtual ~Notificator();
  void Release();
  int Initialize(const shared_ptr<ACE_Auto_Event> &sink_event);

private:
  int Init_Context();
  int Create_IWbemServices();
  int Create_IWbemObjectSink(const shared_ptr<ACE_Auto_Event> &sink_event);
  int Register_Notification();

  const _bstr_t wql_query;
  const _bstr_t app_name;
  bool is_cencel;

  IWbemServices *service_;
  IWbemObjectSink *stub_sink_;
};

static void SvcDebugOut(LPCWSTR String, DWORD Status);

} // namespace WinNT