#include "constants.h"

namespace lic {

#define _CONFIG_FILE_KEYS
#define DAT(a, b) const utility::string_t config_keys::a = b;
#include "../resources/constants.dat"
#undef _CONFIG_FILE_KEYS
#undef DAT

#define _LICENSE_STATUSES
#define DAT(a, b, c) const unsigned short host_states::a;
#include "../resources/constants.dat"
#undef _LICENSE_STATUSES
#undef DAT

#define _CONSTANTS
#define DAT(a, b, c) const static unsigned short a = b;
#include "../resources/constants.dat"
#undef _CONSTANTS
#undef DAT
} // namespace lic
