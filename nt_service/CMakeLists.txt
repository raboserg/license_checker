cmake_minimum_required(VERSION 3.12)
#project(NT_Service)

if(CMAKE_HOST_WIN32)
	#set(VCPKG_DIR "d:/library/vcpkg/packages/ace_x64_windows")
    #set(ACE_LIB_NAME "aced")
		if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		set(ACE_LIB_NAME "ACEd")
	else()
		set(ACE_LIB_NAME "ACE")
	endif()
elseif(CMAKE_HOST_UNIX)
    set(ACE_LIB_NAME "ACE")
endif()
	
#find_library(ACE NAMES ${ACE_LIB_NAME} HINTS ${ACE_LIBRARY_DIR})
#add_executable(${PROJECT_NAME} main.cpp manager.h ntsvc.cpp ntsvc.h notificator.cpp notificator.h eventsink.cpp eventsink.h )

add_library(NT_Service STATIC initializer.cpp initializer.h 
	ntsvc.cpp ntsvc.h 
	notificator.cpp notificator.h 
	eventsink.cpp eventsink.h event_sink_task.cpp event_sink_task.h
	get_license_task.cpp get_license_task.h)

target_link_libraries(NT_Service PUBLIC ${ACE})
target_include_directories(NT_Service PUBLIC ../tracer ../tools ../client ../license ../parser ${ACE_INCLUDE_DIR} ${DEFAULT_LIBP7_INCLUDE_DIR})