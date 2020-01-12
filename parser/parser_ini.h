#ifndef __PARSER_INI_H__
#define __PARSER_INI_H__

//#include <boost/iostreams/filtering_stream.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>

using namespace std;
namespace pt = boost::property_tree;

class Parser {
  pt::ptree root_;
  const string file_name_;

  void create_root(const std::string &file_name);

public:
  Parser(const string &file_name);
  string get_value(const string &key) const;
  pt::ptree get_tree() { return this->root_; }
};
#endif
