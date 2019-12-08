// license_checker.h

#ifndef CLIENT_LICENSE_H
#define CLIENT_LICENSE_H

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/config.hpp>
#include <iostream>
#include <string>

using namespace std;

class BOOST_SYMBOL_VISIBLE my_plugin_api {
public:
  virtual string name() const = 0;
  virtual float calculate(const float x, const float y) = 0;
  virtual ~my_plugin_api() {}
};

class ClientLisence : public my_plugin_api {
public:
  void echo() { std::cout << "ClientLisence" << std::endl; }
};

#endif
