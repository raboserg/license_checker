#pragma once

#ifndef EXEC_NOTIFICATION_H
#define EXEC_NOTIFICATION_H

#include "eventsink.h"
#include <thread>

static const _bstr_t applicationName = "openvpn.exe";

static const _bstr_t wql_query_ =
    "SELECT * "
    "FROM __InstanceCreationEvent WITHIN 1 "
    "WHERE TargetInstance ISA 'Win32_Process' and "
    "TargetInstance.Name = "
    "'openvpn.exe'";

class exec_notificator {
public:
  exec_notificator(const _bstr_t name);
  virtual ~exec_notificator();
  void operator()(const HANDLE event);

private:
  void start_notification_wait();
  std::tuple<bool, CComPtr<IWbemServices>, CComPtr<IWbemObjectSink>> init();
  void release();

  _bstr_t wql_query;
  HANDLE cencel_event;
  const _bstr_t app_name;
};

#endif