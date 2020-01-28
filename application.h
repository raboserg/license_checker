// applicaton.h
#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H
#include "stdafx.h"

class CMainApp {
  std::unique_ptr<Parser> parser_;
  std::unique_ptr<LicenseChecker> licenseChecker_;

public:
  CMainApp(const utility::string_t config_file_name);
  virtual ~CMainApp();
  //???utility::string_t make_verify_license_cmd();
  //???utility::string_t make_machine_uid_cmd();
};

CMainApp::CMainApp(const utility::string_t config_file_name)
    : parser_(std::make_unique<Parser>(config_file_name)),
      licenseChecker_(std::make_unique<LicenseChecker>()) {}

CMainApp::~CMainApp() {}

#endif
