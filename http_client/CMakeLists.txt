cmake_minimum_required(VERSION 3.13.4)
project(http-client)

#SET(CMAKE_C_COMPILER /usr/local/bin/hfuzz-clang)
#SET(CMAKE_CXX_COMPILER /usr/local/bin/hfuzz-clang++)

#set(CMAKE_C_FLAGS "-Wall -Wextra -Wshadow -g -std=gnu11")

set(ACE_ROOT_DIR $ENV{ACE_ROOT})
include_directories(${ACE_ROOT_DIR})

add_executable(${PROJECT_NAME} http_client.cpp)

find_library(ACE_LIBRARY
    NAMES ACE
    HINTS "${ACE_ROOT_DIR}/lib")

target_link_libraries(${PROJECT_NAME} ${ACE_LIBRARY} pthread)
