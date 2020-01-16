cmake_minimum_required (VERSION 3.8)

project(eventsink)
set(SOURCES_LIST notificator_linux.cpp notificator_linux.h)
add_library(eventsink STATIC ${SOURCES_LIST})
#target_link_libraries(${PROJECT_NAME} ${ACE_LIBRARY} pthread)
target_include_directories(${PROJECT_NAME} PUBLIC ../tools ${CMAKE_INCLUDE_PATH} ${DEFAULT_UNIX_LIBP7_DIR}/Headers)

