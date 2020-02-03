#pragma once
#include <iostream>

#ifdef _WIN32
#include "initializer.h"
#else
#include "notificator_linux.h"
#endif

//#include <tools.h>
#include <tracer.h>
#include <constants.h>
