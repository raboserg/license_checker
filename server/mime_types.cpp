//
// mime_types.cpp
//

#include "mime_types.hpp"

namespace http {
namespace server {
namespace mime_types {

static struct mapping {
  const char *extension;
  const char *mime_type;
} mappings[] = {
    {"gif", "image/gif"},  {"htm", "text/html"},
    {"html", "text/html"}, {"jpg", "image/jpeg"},
    {"png", "image/png"},  {"js", "text/js"},
    {"css", "text/css"},   {"javascript", "application/javascript"}};

std::string extension_to_type(const std::string &extension) {
  for (mapping m : mappings) {
    if (m.extension == extension) {
      return m.mime_type;
    }
  }

  return "text/plain";
}

} // namespace mime_types
} // namespace server
} // namespace http
