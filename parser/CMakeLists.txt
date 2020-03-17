cmake_minimum_required (VERSION 3.8)
set(SOURCE_LIST parser_ini.cpp parser_ini.h)
add_library(parser STATIC ${SOURCE_LIST})
#add_executable(parser test.cpp ${SOURCE_LIST}) 
target_include_directories(parser PUBLIC ../tracer ../tools ../constants
    ${Boost_INCLUDE_DIRS} ${CMAKE_INCLUDE_PATH} ${DEFAULT_LIBP7_INCLUDE_DIR})
target_link_libraries(parser PRIVATE tools constants)
