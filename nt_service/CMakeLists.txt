cmake_minimum_required(VERSION 3.12)
#project(NT_Service)

if(CMAKE_HOST_WIN32)
	#set(VCPKG_DIR "d:/library/vcpkg/packages/ace_x64_windows")
    set(ACE_LIB_NAME "aced")
elseif(CMAKE_HOST_UNIX)
    set(ACE_LIB_NAME "ACE")
endif()
	
find_library(ACE NAMES ${ACE_LIB_NAME} HINTS ${ACE_LIBRARY_DIR})
#add_executable(${PROJECT_NAME} main.cpp manager.h ntsvc.cpp ntsvc.h notificator.cpp notificator.h eventsink.cpp eventsink.h )

add_library(NT_Service STATIC initializer.cpp initializer.h 
	ntsvc.cpp ntsvc.h 
	notificator.cpp notificator.h 
	eventsink.cpp eventsink.h worker_task.cpp worker_task.h
	get_task.cpp get_task.h)

target_link_libraries(NT_Service PUBLIC ${ACE})
target_include_directories(NT_Service PUBLIC ../tracer ../tools ../client ../license ../parser ${ACE_INCLUDE_DIR} ${DEFAULT_LIBP7_INCLUDE_DIR})