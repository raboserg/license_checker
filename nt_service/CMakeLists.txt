cmake_minimum_required(VERSION 3.12)
project(NT_Service)
add_library(${PROJECT_NAME} STATIC initializer.cpp initializer.h 
	ntsvc.cpp ntsvc.h 
	notificator.cpp notificator.h 
	eventsink.cpp eventsink.h 
	config_change.cpp config_change.h)
target_link_libraries(${PROJECT_NAME} PRIVATE tracer client license_checker 
get_license_task process_killer_task event_sink_task ${ACE} ${P7_LIB})

target_include_directories(${PROJECT_NAME} PUBLIC ../tracer 
	../tools ../client ../license ../parser ../get_license_task ../common ../event_sink_task
	../process_killer_task ${ACE_INCLUDE_DIR} ${DEFAULT_LIBP7_INCLUDE_DIR})

target_compile_definitions(${PROJECT_NAME} PRIVATE -D_CRT_SECURE_NO_WARNINGS)