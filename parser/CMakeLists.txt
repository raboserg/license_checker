cmake_minimum_required (VERSION 3.8)

set(SOURCE_LIB parser_ini.cpp parser_ini.h)
add_library(parser  STATIC ${SOURCE_LIB})
#add_executable(parser test.cpp ${SOURCE_LIB}) 
target_include_directories(parser PUBLIC ${Boost_INCLUDE_DIRS})