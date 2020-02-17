cmake_minimum_required(VERSION 3.12)
#set(BUILD_SHARED_LIBRARIES OFF)
#set (CMAKE_CXX_STANDARD 11)

if (MSVC)
  add_definitions( "/D_CRT_SECURE_NO_WARNINGS /D_WINSOCK_DEPRECATED_NO_WARNINGS" )
endif(MSVC)

set(LIBS ${LIBS})
if (MSVC)
  set(LIBS ${LIBS} ws2_32.lib)
else()
  set( CMAKE_CXX_FLAGS " -pthread " )
endif(MSVC)

add_library(get_update_task STATIC get_update_task.cpp get_update_task.h socket.cc socket.hh ftp.cc ftp.hh)
target_link_libraries(get_update_task PRIVATE ${ACE})
target_include_directories(get_update_task PRIVATE ../tracer ../tools ../client ../license ../parser
${ACE_INCLUDE_DIR} ${CPPREST_INCLUDE_DIR} ${DEFAULT_LIBP7_INCLUDE_DIR})
target_compile_definitions(get_update_task PRIVATE -D_CRT_SECURE_NO_WARNINGS)


