cmake_minimum_required (VERSION 3.8)
project(tracer)
set(SOURCE_LIST tracer.cpp tracer.h)
add_library(${PROJECT_NAME} STATIC ${SOURCE_LIST})

set(LOG_FILE_CONFIG "/P7.Sink=FileTxt /P7.Dir=${ROOT_PROJECT_BINARY_DIR}/logs ")
set(LOG_SERVER_CONFIG "/P7.Sink=Baical /P7.Pool=32768 /P7.PSize=65536 /P7.Addr=127.0.0.1 /P7:Port=9009")

message("CONFIG_FILECONFIG_FILECONFIG_FILE ${CONFIG_FILE}")

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        target_compile_definitions(${PROJECT_NAME} PRIVATE is_debug=0)
	target_compile_definitions(${PROJECT_NAME} PRIVATE log_config="${LOG_SERVER_CONFIG}")
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE is_debug=0)
	target_compile_definitions(${PROJECT_NAME} PRIVATE log_config="${LOG_FILE_CONFIG}")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${CPPREST_INCLUDE_DIR} ../tools ../parser
    ${ACE_INCLUDE_DIR}
    ${DEFAULT_LIBP7_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE ${ACE})
