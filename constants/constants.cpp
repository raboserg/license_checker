#include "constants.h"

namespace lic {

#define _CONFIG_FILE_KEYS
#define DAT(a, b) const utility::string_t config_keys::a = b;
#include "constants.dat"
#undef _CONFIG_FILE_KEYS
#undef DAT

#define _HOST_STATUSES
#define DAT(a, b, c) const unsigned short lic_host_status::a;
#include "constants.dat"
#undef _HOST_STATUSES
#undef DAT

#define _CONSTANTS
#define DAT(a, b, c) const static long a = b;
#include "constants.dat"
#undef _CONSTANTS
#undef DAT

#define _ERROR_CODE
#define DAT(a, b, c) const static int a = b;
#include "constants.dat"
#undef _ERROR_CODE
#undef DAT
} // namespace lic
