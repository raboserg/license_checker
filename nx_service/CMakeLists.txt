cmake_minimum_required (VERSION 3.8)

project(nx_service)
set(SOURCES_LIST notificator_linux.cpp notificator_linux.h nxsvc.cpp nxsvc.h)
add_library(${PROJECT_NAME} STATIC ${SOURCES_LIST})
target_link_libraries(${PROJECT_NAME} PRIVATE client license_checker get_license_task process_killer_task ${ACE} ${P7_LIB} )
target_include_directories(${PROJECT_NAME} PRIVATE ../tracer ../tools ${CMAKE_INCLUDE_PATH}
    ../client ../license ../parser ../get_license_task ../common
    ../process_killer_task ${ACE_INCLUDE_DIR} ${DEFAULT_UNIX_LIBP7_DIR}/Headers)

