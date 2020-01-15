// applicaton.h
#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H
#include "stdafx.h"

#ifdef _WIN32
static const utility::string_t LIC_INI_FILE = U("lic_check_w.ini");
#else
static const utility::string_t LIC_INI_FILE = U("lic_check_l.ini");
#endif

// class CEtwProvider {
//  CEtwProvider(__in REFGUID ProviderId) { (VOID) Register(ProviderId); }
//
//  HRESULT Register(__in REFGUID ProviderId) {
//    Unregister();
//    return HRESULT_FROM_WIN32(
//        EventRegister(&ProviderId, 0, 0, &m_hProviderHandle));
//  }
//
//  VOID Write(__in const EVENT_DESCRIPTOR &eventDesc) {
//    (VOID) EventWrite(m_hProviderHandle, &eventDesc, 0, NULL);
//  }
//
//  HANDLE m_hProviderHandle;
//};

class CMainApp {
  std::unique_ptr<Parser> parser_;
  std::unique_ptr<LicenseChecker> licenseChecker_;

public:
  CMainApp();
  virtual ~CMainApp();
};

CMainApp::CMainApp()
    : parser_(std::make_unique<Parser>(LIC_INI_FILE)),
      licenseChecker_(std::make_unique<LicenseChecker>()) {}

CMainApp::~CMainApp() {}

#endif
