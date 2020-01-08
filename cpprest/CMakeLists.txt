cmake_minimum_required (VERSION 3.8)


set(SOURCES_LIST http_server.cpp http_server.h Rest_Server.cpp stdafx.h usr_interrupt_handler.hpp)

if (UNIX)
  add_definitions(-Wno-sign-compare -Wno-enum-compare)
endif()

if(CMAKE_HOST_WIN32)
    set(CPPREST_DIR D:/library/vcpkg/packages/cpprestsdk_x64-windows)
    set(CPPREST_LIB_NAME "cpprest_2_10d")
    set(CPPREST_ROOT ${CPPREST_DIR})
    set(CPPREST_HEADER_DIR ${CPPREST_ROOT}/include)
    LINK_DIRECTORIES("${CPPREST_ROOT}/debug/lib")
elseif(CMAKE_HOST_UNIX)
    set(CPPREST_DIR ~/projects/vcpkg/packages/cpprestsdk_x64-linux)
    set(CPPREST_LIB_NAME cpprest)
    set(CPPREST_ROOT ${CPPREST_DIR})
    set(CPPREST_HEADER_DIR ${CPPREST_ROOT}/include)
    LINK_DIRECTORIES("${CPPREST_ROOT}/build.debug/Release/libs")
endif()

#add_library(restserver http_server.cpp http_server.h Rest_Server.cpp stdafx.h)
add_executable(restserver ${SOURCES_LIST})
#find_package(cpprestsdk CONFIG REQUIRED)
if(CMAKE_HOST_WIN32)
	target_link_libraries(restserver ${CPPREST_LIB_NAME})
elseif(CMAKE_HOST_UNIX)
        target_link_libraries(restserver  ${CPPREST_LIB_NAME} pthread boost_system crypto ssl pthread dl)
endif()
#target_link_libraries(restserver pthread ${CPPREST_LIB_NAME})
target_include_directories(restserver PUBLIC ${CPPREST_HEADER_DIR} ${Boost_INCLUDE_DIRS})

# dll of dependencies
#boost_date_time-vc142-mt-gd-x64-1_71.dll
#brotlicommon.dll
#brotlidec.dll
#brotlienc.dll
#cpprest_2_10d.dll
#LIBEAY32.dll
#SSLEAY32.dll
#zlibd1.dll
