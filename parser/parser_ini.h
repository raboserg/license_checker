#ifndef __PARSER_INI_H__
#define __PARSER_INI_H__

//#include <boost/iostreams/filtering_stream.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>

namespace pt = boost::property_tree;

class Parser {
  pt::ptree root_;
  const std::string file_name_;
  //???src::severity_logger< severity_level > lg;

  void create_root(const std::string &file_name);

public:
  Parser(const std::string &file_name);
  std::string get_value(const std::string &key) const;
  pt::ptree get_tree() { return this->root_; }
};
#endif
