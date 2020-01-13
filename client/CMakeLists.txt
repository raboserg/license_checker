cmake_minimum_required (VERSION 3.8)

set(SOURCE_LIB client_license.cpp client_license.h)

set(CPPREST_DIR ${VCPKG_DIR})

if(CMAKE_HOST_WIN32)
    set(CPPREST_LIB_NAME "cpprest_2_10d")
elseif(CMAKE_HOST_UNIX)
    set(CPPREST_LIB_NAME cpprest)
endif()

#add_executable(client ${SOURCE_LIB})
add_library (client STATIC ${SOURCE_LIB})

find_library(CPPREST_LIB NAMES ${CPPREST_LIB_NAME} PATHS ${CMAKE_LIBRARY_PATH})

target_include_directories(client PUBLIC ${CMAKE_INCLUDE_PATH} ${Boost_INCLUDE_DIRS})

if(CMAKE_HOST_WIN32)
    target_link_libraries(client ${CPPREST_LIB}) 
elseif(CMAKE_HOST_UNIX)
    target_link_libraries(client PUBLIC ${CPPREST_LIB} pthread boost_system crypto ssl pthread dl)
endif()
