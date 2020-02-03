#ifndef PARSER_INI_H
#define PARSER_INI_H

#include <tracer.h>
//#define BOOST_NO_CXX11_DECLTYPE
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <cpprest/details/basic_types.h>
#include <cpprest/asyncrt_utils.h>

namespace pt = boost::property_tree;

#ifdef _WIN32
typedef pt::wptree prop_tree;
#else
typedef pt::ptree prop_tree;
#endif

class Parser {
  prop_tree root_;
  const utility::string_t file_name_;

  utility::string_t get_path();
  void create_root(const utility::string_t &file_name);

public:
  Parser();
  Parser(const utility::string_t &file_name);
  utility::string_t get_value(const utility::string_t &key) const;
  prop_tree get_tree() { return this->root_; }
  virtual ~Parser();
};

typedef utils::Singleton<Parser> PARSER;

#endif
