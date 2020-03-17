cmake_minimum_required (VERSION 3.8)

set(SOURCE_LIB license_checker.cpp license_checker.h)
add_library(license_checker STATIC ${SOURCE_LIB})
target_include_directories(license_checker PRIVATE ../client ../parser
    ${Boost_INCLUDE_DIRS} ${DEFAULT_LIBP7_INCLUDE_DIR})
if(CMAKE_HOST_WIN32)
    target_link_libraries(license_checker client parser ${LICENSE} ${Boost_LIBRARIES})
    target_compile_definitions(license_checker PRIVATE -D_CRT_SECURE_NO_WARNINGS -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
elseif(CMAKE_HOST_UNIX)
    target_link_libraries(license_checker client parser ${LICENSE} ${Boost_LIBRARIES})
endif()
