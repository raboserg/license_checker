﻿#include "eventsink.h"

ULONG __stdcall EventSink::AddRef() { return InterlockedIncrement(&m_lRef); }

ULONG __stdcall EventSink::Release() {
  LONG lRef = InterlockedDecrement(&m_lRef);
  if (lRef == 0)
    delete this;
  return lRef;
}

HRESULT __stdcall EventSink::QueryInterface(REFIID riid, void **ppv) {
  if (riid == IID_IUnknown || riid == IID_IWbemObjectSink) {
    *ppv = (IWbemObjectSink *)this;
    AddRef();
    return WBEM_S_NO_ERROR;
  } else
    return E_NOINTERFACE;
}

HRESULT __stdcall EventSink::Indicate(LONG lObjectCount,
                                      IWbemClassObject **apObjArray) {
  HRESULT hres = S_OK;
  BSTR strClassProp = SysAllocString(L"__CLASS");
  for (int i = 0; i < lObjectCount; i++) {



    printf("Event: openvpn is opening\n");
    OutputDebugString(L"Event: openvpn is opening");
    _variant_t varReturnValue;
    hres = (*apObjArray)->Get(strClassProp, 0, &varReturnValue, NULL, 0);
    BSTR dsfdsf = varReturnValue.bstrVal;
    if (SUCCEEDED(hres) && (V_VT(&varReturnValue) == VT_BSTR)) {
      wprintf(L"The class name is %s\n.", V_BSTR(&varReturnValue));
    } else {
      wprintf(L"Error in getting specified object\n");
    }
    VariantClear(&varReturnValue);
  }
  return WBEM_S_NO_ERROR;
}

HRESULT __stdcall EventSink::SetStatus(LONG lFlags, HRESULT hResult,
                                       BSTR strParam,
                                       IWbemClassObject __RPC_FAR *pObjParam) {
  if (lFlags == WBEM_STATUS_COMPLETE) {
    printf("Call complete. hResult = 0x%X\n", hResult);
    OutputDebugString(L"Call complete. hResult");
    m_inProgress = false;
  } else if (lFlags == WBEM_STATUS_PROGRESS) {
    OutputDebugString(L"Call in progress");
    printf("Call in progress.\n");
  }
  return WBEM_S_NO_ERROR;
}