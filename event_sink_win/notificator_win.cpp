#include "notificator_win.h"

WinNT::Notificator::Notificator(const _bstr_t name)
    : app_name(name), is_cencel(false) {
}

WinNT::Notificator ::~Notificator() { release(); };

DWORD WinNT::Notificator::wait(HANDLE hStopEvent) {
  BOOL result;
  WinNT::IWBSRVS pSvc;
  WinNT::IWBOBJSINK pStubSink;
  std::tie(result, pSvc, pStubSink) = registration_event();
  if (result) {
    HRESULT hres = pSvc->ExecNotificationQueryAsync(
        _bstr_t("WQL"), wql_query_, WBEM_FLAG_SEND_STATUS, NULL, pStubSink);
    if (FAILED(hres)) {
      SvcDebugOut(TEXT("ExecNotificationQueryAsync failed with = 0x%X"), hres);
      return 1;
    }
    OutputDebugString(L"ExecNotificationQueryAsync started");
    WaitForSingleObject(hStopEvent, INFINITE);
    pSvc->CancelAsyncCall(pStubSink);
    OutputDebugString(L"ExecNotificationQueryAsync stoped");
    Sleep(1000);
    return 0;
  }
  return 1;
}

std::tuple<BOOL, WinNT::IWBSRVS, WinNT::IWBOBJSINK>
WinNT::Notificator::registration_event() {
  HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
  if (FAILED(hres)) {
    SvcDebugOut(TEXT("Failed to initialize COM library. Error code = = 0x%X"),
                hres);
    return std::make_tuple(false, nullptr, nullptr);
  }
  hres =
      CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
                           RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
  if (FAILED(hres)) {
    SvcDebugOut(TEXT("Failed to initialize security. Error code = 0x%X"), hres);
    return std::make_tuple(false, nullptr, nullptr);
  }

  CComPtr<IWbemLocator> pLoc;
  hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
                          IID_IWbemLocator, (LPVOID *)&pLoc);
  if (FAILED(hres)) {
    SvcDebugOut(TEXT("Failed to create IWbemLocator object. Error code = 0x%X"),
                hres);
    return std::make_tuple(false, nullptr, nullptr);
  }

  CComPtr<IWbemServices> pSvc;
  hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0,
                             &pSvc);
  if (FAILED(hres)) {
    SvcDebugOut(TEXT("Could not connect. Error code = 0x%X"), hres);
    return std::make_tuple(false, nullptr, nullptr);
  }
  SvcDebugOut(L"Connected to ROOT\\CIMV2 WMI namespace", 0);
  hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                           RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
                           NULL, EOAC_NONE);
  if (FAILED(hres)) {
    SvcDebugOut(TEXT("Could not set proxy blanket. Error code = 0x%X"), hres);
    return std::make_tuple(false, nullptr, nullptr);
  }
  CComPtr<IUnsecuredApartment> pUnsecApp;
  hres = CoCreateInstance(CLSID_UnsecuredApartment, NULL, CLSCTX_LOCAL_SERVER,
                          IID_IUnsecuredApartment, (void **)&pUnsecApp);

  CComPtr<IUnknown> pStubUnk;
  CComPtr<IWbemObjectSink> pSink(new EventSink);
  pUnsecApp->CreateObjectStub(pSink, &pStubUnk);

  CComPtr<IWbemObjectSink> pStubSink;
  pStubUnk->QueryInterface(IID_IWbemObjectSink, (void **)&pStubSink);
  return std::make_tuple(true, pSvc, pStubSink);
}

void WinNT::Notificator::set_cencel(const bool flag) { this->is_cencel = flag; }

void WinNT::Notificator::release() {
  cout << "release" << endl;
  CoUninitialize();
}

void WinNT::SvcDebugOut(LPCWSTR message, DWORD status) {
  const size_t cchDest = 100;
  TCHAR pszDest[cchDest];
  ::StringCchPrintf(pszDest, cchDest, message, status);
  OutputDebugString(pszDest);
}
