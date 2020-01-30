cmake_minimum_required (VERSION 3.8)
set(SOURCE_LIST tracer.cpp tracer.h)

add_library(tracer STATIC ${SOURCE_LIST})

set(ACE_ROOT ${VCPKG_DIR})
set(ACE_INCLUDE_DIR "${ACE_ROOT}/include")
set(ACE_LIBRARY_DIR "${ACE_ROOT}/Debug/lib")

find_library(ACE NAMES aced HINTS ${ACE_LIBRARY_DIR})

target_include_directories(tracer PUBLIC ../tools ${ACE_LIBRARY_DIR} ${DEFAULT_LIBP7_INCLUDE_DIR})
target_link_libraries(tracer PRIVATE ${ACE})