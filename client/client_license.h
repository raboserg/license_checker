﻿// client_license.h

#ifndef CLIENT_LICENSE_H
#define CLIENT_LICENSE_H

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/config.hpp>
//#include <cpprest/http_client.h>
//#include <cpprest/json.h>
//#include <cpprest/uri.h>
#include <iostream>
#include <string>

class BOOST_SYMBOL_VISIBLE my_plugin_api {
public:
  virtual std::string name() const = 0;
  virtual float calculate(const float x, const float y) = 0;
  virtual ~my_plugin_api() {}
};

class ClientLisence : public my_plugin_api {
public:
  void echo() { std::cout << "ClientLisence" << std::endl; }
};

int main_run();

#endif
