cmake_minimum_required(VERSION 3.12)
add_library(process_killer_task STATIC process_killer_task.cpp process_killer_task.h)
target_link_libraries(process_killer_task PRIVATE ${ACE})
target_include_directories(process_killer_task PRIVATE ../tracer ../tools ../client ../license ../parser
    ${ACE_INCLUDE_DIR} ${CPPREST_INCLUDE_DIR} ${DEFAULT_LIBP7_INCLUDE_DIR})
target_compile_definitions(process_killer_task PRIVATE -D_CRT_SECURE_NO_WARNINGS)
