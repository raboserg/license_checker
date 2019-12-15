#ifndef MIME_TYPES_H
#define MIME_TYPES_H

#include <boost/beast.hpp>

namespace beast = boost::beast; // from <boost/beast.hpp>

namespace mime_types {

beast::string_view mime_type(beast::string_view path);

} // namespace mime_types

#endif // MIME_TYPES_H
