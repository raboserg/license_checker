cmake_minimum_required (VERSION 3.8)

set(SOURCE_LIST constants.cpp constants.h)

add_library(constants STATIC ${SOURCE_LIST})

target_include_directories(constants PRIVATE ${CPPREST_INCLUDE_DIR})
#arget_link_libraries(tracer PRIVATE ${ACE})
