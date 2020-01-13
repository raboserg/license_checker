#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

namespace license_checker {

static const std::string log_name = "sample_%N.log";
static const size_t rotation_size = 10 * 1024 * 1024;
static const std::string format = "[%TimeStamp%]: %Message%";

void init_logger() {

  logging::add_file_log(keywords::file_name = log_name,
                        keywords::rotation_size = rotation_size,
                        keywords::time_based_rotation =
                            sinks::file::rotation_at_time_point(0, 0, 0),
                        keywords::format = format);

  logging::core::get()->set_filter(logging::trivial::severity >=
                                   logging::trivial::info);
}
struct LogInfo {
  LogInfo() {}
  LogInfo(std::string str) : text(std::move(str)) {}
  std::string text; // log output (usually but not always one line)
};
} // namespace license_checker