#pragma once
#ifndef EVENTSINK_H
#define EVENTSINK_H

#include <ace/Auto_Event.h>

#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <Wbemidl.h>
#include <comdef.h>

#include <atlbase.h>

#pragma comment(lib, "wbemuuid.lib")

class EventSink : public IWbemObjectSink {
  LONG m_lRef;
  bool bDone;

public:
  EventSink(const shared_ptr<ACE_Auto_Event> &sink_event)
      : in_progress_(true), sink_event_(sink_event) {
    m_lRef = 0;
  }
  ~EventSink() { bDone = true; }
  bool in_progress_;
  shared_ptr<ACE_Auto_Event> sink_event_;

  virtual ULONG __stdcall AddRef();
  virtual ULONG __stdcall Release();
  virtual HRESULT __stdcall QueryInterface(REFIID riid, void **ppv);

  virtual HRESULT __stdcall Indicate(
      LONG lObjectCount, IWbemClassObject __RPC_FAR *__RPC_FAR *apObjArray);

  virtual HRESULT __stdcall SetStatus(LONG lFlags, HRESULT hResult,
                                      BSTR strParam,
                                      IWbemClassObject __RPC_FAR *pObjParam);
};

#endif // end of EventSink.h