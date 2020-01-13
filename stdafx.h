#pragma once

#include "client_license.h"
#include "license_checker.h"
#include "platform.hpp"
//#include <cpprest/http_client.h>
//#include <cpprest/json.h>
//#include <cpprest/uri.h>
#include <iostream>
#include <thread>

#ifdef _WIN32
#include "service.h"
#else
#include "notificator_linux.h"
#endif

#include "tools.h"
//#include "logger.h"

#include <P7_Telemetry.h>
#include <P7_Trace.h>
