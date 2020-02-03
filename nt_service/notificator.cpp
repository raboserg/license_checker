#include "notificator.h"
#include <roapi.h>
#include "tracer.h"

WinNT::Notificator::Notificator()
    : app_name(applicationName), is_cencel(false) {
  // Initialize();
  DEBUG_LOG(TM("Notificator::Notificator()"));
}

WinNT::Notificator::Notificator(const _bstr_t name)
    : app_name(name), is_cencel(false) {
  // Initialize();
}

WinNT::Notificator::~Notificator(){
    // Release();
};

int WinNT::Notificator::Register_Notification() {
  HRESULT hres = this->service_->ExecNotificationQueryAsync(
      _bstr_t("WQL"), wql_query_, WBEM_FLAG_SEND_STATUS, NULL,
      this->stub_sink_);
  if (FAILED(hres)) {
    SvcDebugOut(TEXT("ExecNotificationQueryAsync failed with = 0x%X"), hres);
    return -1;
  }
  return 0;
}

int WinNT::Notificator::Create_IWbemServices() {
  CComPtr<IWbemLocator> locator;
  HRESULT hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
                                  IID_IWbemLocator, (LPVOID *)&locator);
  if (FAILED(hres)) {
    SvcDebugOut(TEXT("Failed to create IWbemLocator object. Error code = 0x%X"),
                hres);
    return -1;
  }

  hres = locator->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0,
                                0, &this->service_);
  if (FAILED(hres)) {
    SvcDebugOut(TEXT("Could not connect. Error code = 0x%X"), hres);
    return -1;
  }
  SvcDebugOut(L"Connected to ROOT\\CIMV2 WMI namespace", 0);
  hres = CoSetProxyBlanket(this->service_, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
                           NULL, RPC_C_AUTHN_LEVEL_CALL,
                           RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
  if (FAILED(hres)) {
    SvcDebugOut(TEXT("Could not set proxy blanket. Error code = 0x%X"), hres);
    return -1;
  }
  return 0;
}

int WinNT::Notificator::Create_IWbemObjectSink(
    const shared_ptr<ACE_Auto_Event> &sink_event) {
  CComPtr<IUnsecuredApartment> pUnsecApp;
  HRESULT hres =
      CoCreateInstance(CLSID_UnsecuredApartment, NULL, CLSCTX_LOCAL_SERVER,
                       IID_IUnsecuredApartment, (void **)&pUnsecApp);

  if (FAILED(hres)) {
    SvcDebugOut(
        TEXT("Failed to initialize IUnsecuredApartment. Error code = 0x%X"),
        hres);
    return -1;
  }

  CComPtr<IUnknown> pStubUnk;
  CComPtr<IWbemObjectSink> pSink = new EventSink(sink_event);
  hres = pUnsecApp->CreateObjectStub(pSink, &pStubUnk);
  if (FAILED(hres)) {
    SvcDebugOut(TEXT("Failed to create CreateObjectStub IWbemObjectSink. Error "
                     "code = 0x%X"),
                hres);
    return -1;
  }

  hres =
      pStubUnk->QueryInterface(IID_IWbemObjectSink, (void **)&this->stub_sink_);
  if (FAILED(hres)) {
    SvcDebugOut(
        TEXT("Failed to QueryInterface IWbemObjectSink. Error code = 0x%X"),
        hres);
    return -1;
  }
  return 0;
}

int WinNT::Notificator::Initialize(
    const shared_ptr<ACE_Auto_Event> &sink_event) {
  if (Init_Context() == -1)
    return -1;
  if (Create_IWbemServices() == -1)
    return -1;
  if (Create_IWbemObjectSink(sink_event) == -1)
    return -1;
  if (Register_Notification() == -1)
    return -1;
  return 0;
}

int WinNT::Notificator::Init_Context() {
  HRESULT hres = Windows::Foundation::Initialize(RO_INIT_MULTITHREADED);
  if (FAILED(hres)) {
    SvcDebugOut(
        TEXT("Failed to Windows::Foundation::Initialize. Error code = = 0x%X"),
        hres);
    return -1;
  }

  hres =
      CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
                           RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
  if (FAILED(hres)) {
    Windows::Foundation::Uninitialize();
    SvcDebugOut(TEXT("Failed to initialize security. Error code = 0x%X"), hres);
    return -1;
  }

  TRACE_LOG(TM("Notificator::Init_Context"));

  return 0;
}

void WinNT::Notificator::set_cencel(const bool flag) { this->is_cencel = flag; }

void WinNT::Notificator::Release() {
  service_->CancelAsyncCall(stub_sink_);
  service_->Release();
  stub_sink_->Release();
  Windows::Foundation::Uninitialize();
  cout << "release" << endl;
}

void WinNT::SvcDebugOut(LPCWSTR message, DWORD status) {
  const size_t cchDest = 100;
  TCHAR pszDest[cchDest];
  ::StringCchPrintf(pszDest, cchDest, message, status);
  OutputDebugString(pszDest);
}
