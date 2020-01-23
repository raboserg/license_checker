#pragma once

#include "cpprest/details/basic_types.h"

namespace lic {
class license_constanst {
public:
#define _LIC_CONSTANTS
#define DAT(a, b) _ASYNCRTIMP const static utility::string_t a;
#include "../resources/constants.dat"
#undef _LIC_CONSTANTS
#undef DAT
};
} // namespace lic
