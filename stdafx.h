#pragma once

#include <thread>
#include <chrono>
#include <iostream>

#include <parser_ini.h>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/asyncrt_utils.h>
#include <cpprest/details/basic_types.h>

#ifdef _WIN32
#include "service.h"
#else
#include "notificator_linux.h"
#endif

#include <tools.h>
#include <boost/exception/all.hpp>
#include <tracer.h>
#include <constants.h>
#include <license_helper.h>

#include "client_license.h"
#include "license_checker.h"
