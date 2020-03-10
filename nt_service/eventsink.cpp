#include "eventsink.h"
#include "event_sink_task.h"
#include "tracer.h"

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
    // printf("Event: openvpn is opening\n");

    /*ACE_Message_Block *mblk = 0;
    ACE_Message_Block *log_blk =
        new ACE_Message_Block(reinterpret_cast<char *>(this));
    log_blk->cont(mblk);
    LICENSE_WORKER_TASK::instance()->put(log_blk);*/

    LICENSE_WORKER_TASK::instance()->open();

    TRACE_LOG(TM("Event: openvpn is opening...\n"));
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T Event: openvpn is opening... (%t) \n")));
    // OutputDebugString(L"Event: openvpn is opening");
    _variant_t varReturnValue;
    hres = (*apObjArray)->Get(strClassProp, 0, &varReturnValue, NULL, 0);
    if (SUCCEEDED(hres) && (V_VT(&varReturnValue) == VT_BSTR)) {
      const std::string strFromBstr =
          (const char *)_bstr_t(V_BSTR(&varReturnValue));

      ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T The class name is %s (%t) \n"),
                 (const char *)_bstr_t(V_BSTR(&varReturnValue))));
    } else {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("%T (%t):\t\tError in getting specified object\n"),
                 V_BSTR(&varReturnValue)));
    }
    VariantClear(&varReturnValue);
  }
  return WBEM_S_NO_ERROR;
}

HRESULT __stdcall EventSink::SetStatus(LONG lFlags, HRESULT hResult,
                                       BSTR strParam,
                                       IWbemClassObject __RPC_FAR *pObjParam) {
  if (lFlags == WBEM_STATUS_COMPLETE) {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("%T (%t):\t\tCall complete. hResult = 0x%X\n"),
               hResult));
    in_progress_ = false;
  } else if (lFlags == WBEM_STATUS_PROGRESS) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("%T (%t):\t\tCall in progress\n"), hResult));
  }
  return WBEM_S_NO_ERROR;
}