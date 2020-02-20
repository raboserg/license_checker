cmake_minimum_required(VERSION 3.12)
#project(NT_Service)
add_library(NT_Service STATIC initializer.cpp initializer.h 
	ntsvc.cpp ntsvc.h 
	notificator.cpp notificator.h 
	eventsink.cpp eventsink.h event_sink_task.cpp event_sink_task.h)
target_link_libraries(NT_Service PRIVATE client license_checker get_license_task process_killer_task ${ACE})
target_include_directories(NT_Service PRIVATE ../tracer 
	../tools ../client ../license ../parser ../get_license_task 
	../process_killer_task ${ACE_INCLUDE_DIR} ${DEFAULT_LIBP7_INCLUDE_DIR})
target_compile_definitions(NT_Service PRIVATE -D_CRT_SECURE_NO_WARNINGS)
