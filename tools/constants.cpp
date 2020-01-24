#pragma once

#include "constants.h"

namespace lic {

#define _CONFIG_FILE_KEYS
#define DAT(a, b) const utility::string_t config_file_keys::a = b;
#include "../resources/constants.dat"
#undef _CONFIG_FILE_KEYS
#undef DAT

#define _HOST_STATUS
#define DAT(a, b) const utility::string_t license_status::a = b;
#include "../resources/constants.dat"
#undef _HOST_STATUS
#undef DAT

} // namespace lic
