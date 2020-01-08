cmake_minimum_required (VERSION 3.8)

project(eventsink)

set(SOURCES_LIST notificator_linux.cpp notificator_linux.h)

add_library(eventsink STATIC ${SOURCES_LIST})

if($ENV{VCPKG_DIR})
    set(VCPKG_DIR $ENV{VCPKG_DIR})
else()
    set(VCPKG_DIR "~/projects/vcpkg/installed/x64-linux")
endif()
#target_link_libraries(eventsink client ${ACE_LIBRARY} )
set(ACE_ROOT ${VCPKG_DIR})
set(ACE_INCLUDE_DIR "${ACE_ROOT}/include")
#set(ACE_LIBRARY_DIR "${ACE_ROOT}/debug/lib")
set(ACE_LIBRARY_DIR "${ACE_ROOT}/lib")

#find_library(ACE_LIBRARY
#        NAMES ACE
#        HINTS ${ACE_LIBRARY_DIR})

#target_link_libraries(${PROJECT_NAME} ${ACE_LIBRARY} pthread)
target_include_directories(${PROJECT_NAME} PUBLIC ${ACE_INCLUDE_DIR})

