#ifndef __PARSER_INI_H__
#define __PARSER_INI_H__

#include <tools.h>
#include <P7_Trace.h>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>

namespace pt = boost::property_tree;

class Parser {
  pt::ptree root_;
  const std::string file_name_;
  IP7_Trace *l_iTrace = nullptr;
  void create_root(const std::string &file_name);

public:
  Parser(const std::string &file_name);
  std::string get_value(const std::string &key) const;
  pt::ptree get_tree() { return this->root_; }
  virtual ~Parser() {
    if (l_iTrace) {
      l_iTrace->Release();
      l_iTrace = NULL;
    }
  }
};
#endif
