#pragma once

#include "client_license.h"
#include "license_checker.h"

#include <thread>
#include <chrono>
#include <iostream>


#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/uri.h>
#include <cpprest/asyncrt_utils.h>
#include <cpprest/details/basic_types.h>

#ifdef _WIN32
#include "service.h"
#else
#include "notificator_linux.h"
#endif

#include <tools.h>
#include <boost/exception/all.hpp>
#include <parser_ini.h>
#include <tracer.h>
#include <constants.h>
#include <license_helper.h>
