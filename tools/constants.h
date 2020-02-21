#pragma once

#include "cpprest/details/basic_types.h"

namespace lic {
struct config_keys {
#define _CONFIG_FILE_KEYS
#define DAT(a, b) const static utility::string_t a;
#include "../resources/constants.dat"
#undef _CONFIG_FILE_KEYS
#undef DAT
};

struct lic_host_status {
#define _HOST_STATUSES
#define DAT(a, b, c) const static unsigned short a = b;
#include "../resources/constants.dat"
#undef _HOST_STATUSES
#undef DAT
};

struct constants {
#define _CONSTANTS
#define DAT(a, b, c) const static int a = b;
#include "../resources/constants.dat"
#undef _CONSTANTS
#undef DAT
};
} // namespace lic
