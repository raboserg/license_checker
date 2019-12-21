#include "exec_notificator.h"

exec_notificator::exec_notificator(const _bstr_t name) : app_name(name) {
  _bstr_t wql_query = "SELECT * "
                      "FROM __InstanceCreationEvent WITHIN 1 "
                      "WHERE TargetInstance ISA 'Win32_Process' and "
                      "TargetInstance.Name = "
                      "'" +
                      name + "'";
  cout << wql_query_ << endl;
};

exec_notificator ::~exec_notificator() { release(); };

void exec_notificator::operator()(const HANDLE event) {
  cencel_event = event;
  start_notification_wait();
}

void exec_notificator::start_notification_wait() {
  bool result;
  CComPtr<IWbemServices> pSvc;
  CComPtr<IWbemObjectSink> pStubSink;
  std::tie(result, pSvc, pStubSink) = init();
  if (result) {
    HRESULT hres = pSvc->ExecNotificationQueryAsync(
        _bstr_t("WQL"), wql_query_, WBEM_FLAG_SEND_STATUS, NULL, pStubSink);
    if (FAILED(hres)) {
      printf("ExecNotificationQueryAsync failed with = 0x%X\n", hres);
      return;
    }
    ::WaitForSingleObject(cencel_event, INFINITE);
    cout << "reset event" << endl;
    pSvc->CancelAsyncCall(pStubSink);
  }
}

std::tuple<bool, CComPtr<IWbemServices>, CComPtr<IWbemObjectSink>>
exec_notificator::init() {

  HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
  if (FAILED(hres)) {
    cout << "Failed to initialize COM library. Error code = 0x" << hex << hres
         << endl;
    return std::make_tuple(false, nullptr, nullptr);
  }
  hres =
      CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
                           RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
  if (FAILED(hres)) {
    cout << "Failed to initialize security. Error code = 0x" << hex << hres
         << endl;
    return std::make_tuple(false, nullptr, nullptr);
  }

  CComPtr<IWbemLocator> pLoc;
  CComPtr<IWbemServices> pSvc;
  hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
                          IID_IWbemLocator, (LPVOID *)&pLoc);
  if (FAILED(hres)) {
    cout << "Failed to create IWbemLocator object. "
         << "Err code = 0x" << hex << hres << endl;
    return std::make_tuple(false, nullptr, nullptr);
  }

  hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0,
                             &pSvc);
  if (FAILED(hres)) {
    cout << "Could not connect. Error code = 0x" << hex << hres << endl;
    return std::make_tuple(false, nullptr, nullptr);
  }
  cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;
  hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                           RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
                           NULL, EOAC_NONE);
  if (FAILED(hres)) {
    cout << "Could not set proxy blanket. Error code = 0x" << hex << hres
         << endl;
    return std::make_tuple(false, nullptr, nullptr);
  }
  CComPtr<IUnsecuredApartment> pUnsecApp;
  hres = CoCreateInstance(CLSID_UnsecuredApartment, NULL, CLSCTX_LOCAL_SERVER,
                          IID_IUnsecuredApartment, (void **)&pUnsecApp);
  CComPtr<IUnknown> pStubUnk;
  CComPtr<IWbemObjectSink> pStubSink;
  CComPtr<IWbemObjectSink> pSink(new EventSink);
  pUnsecApp->CreateObjectStub(pSink, &pStubUnk);
  pStubUnk->QueryInterface(IID_IWbemObjectSink, (void **)&pStubSink);
  return std::make_tuple(true, pSvc, pStubSink);
}

void exec_notificator::release() {
  cout << "release" << endl;
  CoUninitialize();
}
