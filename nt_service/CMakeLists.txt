cmake_minimum_required(VERSION 3.12)

project(NT_Service)

if($ENV{VCPKG_DIR})
    set(VCPKG_DIR $ENV{VCPKG_DIR})
else()
    if(CMAKE_HOST_WIN32)
        #set(VCPKG_DIR "d:/library/vcpkg/packages/ace_x64_windows")
        set(VCPKG_DIR "d:/library/vcpkg/installed/x64-windows")
        set(ACE_LIB_NAME "aced")
    elseif(CMAKE_HOST_UNIX)
        set(VCPKG_DIR "~/projects/vcpkg/installed/x64-linux")
        set(ACE_LIB_NAME "ACE")
    endif()
	
	set(ACE_ROOT ${VCPKG_DIR})
    set(ACE_INCLUDE_DIR "${ACE_ROOT}/include")
    set(ACE_LIBRARY_DIR "${ACE_ROOT}/Debug/lib")
endif()

#LINK_DIRECTORIES(${ACE_LIBRARY_DIR})


find_library(ACE NAMES ${ACE_LIB_NAME} HINTS ${ACE_LIBRARY_DIR})
#add_executable(${PROJECT_NAME} main.cpp manager.h ntsvc.cpp ntsvc.h notificator.cpp notificator.h eventsink.cpp eventsink.h )

add_library(eventsink STATIC eventsink.cpp eventsink.h)
target_link_libraries(eventsink PRIVATE ${ACE})
target_include_directories(eventsink PUBLIC ../tracer ../tools ${ACE_INCLUDE_DIR} ${DEFAULT_LIBP7_INCLUDE_DIR})

add_library(notificator STATIC notificator.cpp notificator.h)
target_link_libraries(notificator PRIVATE eventsink ${ACE})
target_include_directories(notificator PUBLIC ../tracer ../tools ${ACE_INCLUDE_DIR} ${DEFAULT_LIBP7_INCLUDE_DIR})

add_library(${PROJECT_NAME} STATIC main.cpp manager.h ntsvc.cpp ntsvc.h)
target_link_libraries(${PROJECT_NAME} PUBLIC eventsink notificator ${ACE})
target_include_directories(${PROJECT_NAME} PUBLIC ../tracer ../tools ${ACE_INCLUDE_DIR} ${DEFAULT_LIBP7_INCLUDE_DIR})
