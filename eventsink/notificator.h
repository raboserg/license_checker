#pragma once

#ifndef EXEC_NOTIFICATION_H
#define EXEC_NOTIFICATION_H

#include "eventsink.h"
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

typedef CComPtr<IWbemServices> IWBSRVS;
typedef CComPtr<IWbemObjectSink> IWBOBJSINK;

class Notificator {

public:
  Notificator(const _bstr_t name);
  virtual ~Notificator();
  DWORD notification_wait(HANDLE hStopEven);
  void set_cencel(const bool flag);

private:
  void release();
  std::tuple<BOOL, IWBSRVS, IWBOBJSINK> init_context();

  const _bstr_t wql_query;
  const _bstr_t app_name;
  bool is_cencel;
};

void SvcDebugOut(LPCWSTR String, DWORD Status);

} // namespace WinNT
#endif