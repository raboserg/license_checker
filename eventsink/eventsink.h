#pragma once
#ifndef EVENTSINK_H
#define EVENTSINK_H

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
  EventSink() : m_inProgress(true) { m_lRef = 0; }
  ~EventSink() { bDone = true; }
  bool m_inProgress;

  virtual ULONG STDMETHODCALLTYPE AddRef();
  virtual ULONG STDMETHODCALLTYPE Release();
  virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppv);

  virtual HRESULT STDMETHODCALLTYPE Indicate(
      LONG lObjectCount, IWbemClassObject __RPC_FAR *__RPC_FAR *apObjArray);

  virtual HRESULT STDMETHODCALLTYPE SetStatus(
      /* [in] */ LONG lFlags,
      /* [in] */ HRESULT hResult,
      /* [in] */ BSTR strParam,
      /* [in] */ IWbemClassObject __RPC_FAR *pObjParam);
};

#endif // end of EventSink.h