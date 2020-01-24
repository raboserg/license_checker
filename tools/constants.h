#pragma once

#include "cpprest/details/basic_types.h"

namespace lic {
struct config_file_keys {
#define _CONFIG_FILE_KEYS
#define DAT(a, b) const static utility::string_t a;
#include "../resources/constants.dat"
#undef _CONFIG_FILE_KEYS
#undef DAT
};

struct license_status {
#define _HOST_STATUS
#define DAT(a, b) const static utility::string_t a;
#include "../resources/constants.dat"
#undef _HOST_STATUS
#undef DAT
};
} // namespace lic
