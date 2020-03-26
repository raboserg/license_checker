cmake_minimum_required (VERSION 3.8)

project(nx_service)

set(SOURCES_LIST notificator_linux.cpp notificator_linux.h
    nxsvc.cpp nxsvc.h 
    config_change.cpp config_change.h 
    itvpn_exec_handler.cpp itvpn_exec_handler.h)

add_library(${PROJECT_NAME} STATIC ${SOURCES_LIST})

target_link_libraries(${PROJECT_NAME} PRIVATE tracer client license_checker
    get_license_task process_killer_task event_sink_task ${ACE} ${P7_LIB} )

target_include_directories(${PROJECT_NAME} PRIVATE ../tracer ../tools ${CMAKE_INCLUDE_PATH}
    ../client ../license ../parser ../get_license_task ../common ../event_sink_task
    ../process_killer_task ${ACE_INCLUDE_DIR} ${DEFAULT_UNIX_LIBP7_DIR}/Headers)

