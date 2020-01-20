#pragma once

#include "client_license.h"
#include "license_checker.h"

#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/uri.h>
#include <iostream>
#include <thread>
//#include <pplx/threadpool.h>

#ifdef _WIN32
#include "service.h"
#else
#include "notificator_linux.h"
#endif

#include "tools.h"
#include <boost/exception/all.hpp>
#include <cpprest/asyncrt_utils.h>
#include <cpprest/details/basic_types.h>
#include <parser_ini.h>
#include <tracer.h>
