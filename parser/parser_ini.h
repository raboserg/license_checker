#ifndef PARSER_INI_H
#define PARSER_INI_H

#include <Singleton.h>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cpprest/details/basic_types.h>

namespace itvpnagent {
namespace pt = boost::property_tree;

#ifdef _WIN32
typedef pt::wptree prop_tree;
#else
typedef pt::ptree prop_tree;
#endif

using namespace utility;

struct Options {
  string_t unp;
  string_t prod;
  string_t lic_app;
  string_t agentId;
  string_t log_type;
  string_t lic_file;
  string_t service_path;
  string_t make_uid_cmd;
  string_t uid_file_name;
  string_t lic_file_name;
  string_t lic_files_path;
  string_t kill_file_name;
  string_t log_files_path;
  long day_license_check;
  long day_license_update;
  string_t openvpn_file_path;
  int next_day_waiting_hours;
  string_t license_manager_uri;
  int next_try_get_license_mins;
};

class Parser {
  prop_tree root_;
  Options options_;
  utility::string_t file_name_;
  utility::string_t service_path_;
  utility::string_t log_file_path_;
  utility::string_t config_file_path_;
  utility::string_t make_service_path();
  prop_tree get_tree() { return this->root_; }
  void create_root(const utility::string_t &file_name);
  void set_options(Options options) { options_ = options; }
  utility::string_t get_value(const utility::string_t &key) const;

public:
  Parser(void);
  int init();
  utility::string_t get_service_path();
  Options options() { return options_; }
  utility::string_t get_log_file_path();
  utility::string_t get_config_file_name();
  utility::string_t get_config_file_path();
  utility::string_t make_config_file_path();
  Parser(const utility::string_t &file_name);
  int make_paths();
  // int init(const utility::string_t &path);
};

typedef Singleton<Parser> PARSER;

#define CONFIG PARSER::instance()->options()

} // namespace itvpnagent
#endif
