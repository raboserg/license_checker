cmake_minimum_required (VERSION 3.8)

set(SOURCE_LIST tracer.cpp tracer.h)

add_library(tracer STATIC ${SOURCE_LIST})

if(CMAKE_HOST_WIN32)
    set(ACE_ROOT ${VCPKG_DIR})
    set(ACE_INCLUDE_DIR "${ACE_ROOT}/include")
    set(ACE_LIBRARY_DIR "${ACE_ROOT}/Debug/lib")
    set(ACE_LIB_NAME "aced")
else()
    set(ACE_LIB_NAME "ACE")
endif()

find_library(ACE_LIBRARY NAMES ${ACE_LIB_NAME} PATHS ${ACE_LIBRARY_DIR})

message("ACE_LIBRARY_DIRACE_LIBRARY_DIRACE_LIBRARY_DIRACE_LIBRARY_DIR ${ACE_LIBRARY_DIR}")
target_include_directories(tracer PUBLIC ../tools ${ACE_LIBRARY_DIR} ${DEFAULT_LIBP7_INCLUDE_DIR})
target_link_libraries(tracer PRIVATE ${ACE_LIBRARY})
