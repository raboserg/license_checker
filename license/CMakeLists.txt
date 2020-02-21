cmake_minimum_required (VERSION 3.8)

set(SOURCE_LIB license_checker.cpp license_checker.h)
add_library(license_checker STATIC ${SOURCE_LIB})
target_link_libraries(license_checker client parser ${LICENSE})
target_include_directories(license_checker PUBLIC ../client ../parser
    ${Boost_INCLUDE_DIRS} ${DEFAULT_LIBP7_INCLUDE_DIR})
target_compile_definitions(license_checker PRIVATE -D_CRT_SECURE_NO_WARNINGS)
