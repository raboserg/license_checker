cmake_minimum_required (VERSION 3.8)

set(SOURCE_LIB client_license.cpp client_license.h)

add_library (client STATIC ${SOURCE_LIB})
target_include_directories(client PUBLIC ${Boost_INCLUDE_DIRS})
