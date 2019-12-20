cmake_minimum_required (VERSION 3.8)

set(SOURCE_LIB license_checker.cpp license_checker.h)
add_library(license STATIC ${SOURCE_LIB})
#target_link_libraries(license client)
target_include_directories(license PUBLIC ${Boost_INCLUDE_DIRS})
