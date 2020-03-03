cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 14)

include(path.cmake)

if(CMAKE_HOST_WIN32)
    project ("itVPNAgent")
elseif(CMAKE_HOST_UNIX)
    project ("itvpnagent")
endif()


#set(LINK "STATIC")
set(LINK "SHARED")

#set(VARIANT "RELEASE")
set(VARIANT "DEBUG")

set(CMAKE_USER_MAKE_RULES_OVERRIDE "build.cmake")
include(build.cmake)

message(STATUS "$$$$$ CMAKE_CROSSCOMPILING - ${CMAKE_CROSSCOMPILING}")
message(STATUS "$$$$$ CMAKE_INCLUDE_PATH - ${CMAKE_INCLUDE_PATH}")
message(STATUS "$$$$$ CMAKE_LIBRARY_PATH - ${CMAKE_LIBRARY_PATH}")

if(CMAKE_HOST_WIN32)
    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set(Boost_USE_DEBUG_LIBS ON)
        set(CPPREST_LIB_NAME "cpprest_2_10d")
        set(P7_LIB_NAME "P7x64d.dll")
        set(ACE_LIB_NAME "ACEd")
    else()
        set(Boost_USE_RELEASE_LIBS ON)
        set(CPPREST_LIB_NAME "cpprest_2_10")
        set(P7_LIB_NAME "P7x64.dll")
        set(ACE_LIB_NAME "ACE")
    endif()
elseif(CMAKE_HOST_UNIX)
    set(CPPREST_LIB_NAME cpprest)
    set(ACE_LIB_NAME "ACE")
endif()

if(CMAKE_HOST_WIN32)
    find_package(Boost 1.71.0 COMPONENTS filesystem REQUIRED)
    find_library(P7_LIB NAMES ${P7_LIB_NAME} HINTS ${DEFAULT_LIBP7_LIB_DIR})
    find_library(CPPREST_LIB NAMES ${CPPREST_LIB_NAME} HINTS ${VCPKG_DIR}/lib)
    find_library(ACE NAMES ${ACE_LIB_NAME} HINTS ${ACE_LIBRARY_DIR})
	#find_library(LICENSE license HINTS "D:/library/vcpkg/installed/x64-windows-static/debug/lib")
elseif(CMAKE_HOST_UNIX)
    find_library(ACE NAMES ${ACE_LIB_NAME} PATHS ${ACE_LIBRARY_DIR})
    set(CMAKE_LIBRARY_PATH_ ${CMAKE_LIBRARY_PATH})
    set(CMAKE_INCLUDE_PATH_ ${CMAKE_INCLUDE_PATH})
    set(CMAKE_LIBRARY_PATH ${DEFAULT_UNIX_BOOST_DIR}/stage/lib)
    set(CMAKE_INCLUDE_PATH ${DEFAULT_UNIX_BOOST_DIR})
    find_package(Boost COMPONENTS system)
    set(CMAKE_LIBRARY_PATH ${DEFAULT_UNIX_LIBP7_DIR}/Binaries)
    find_library(P7_LIB NAMES P7)
    file(COPY ${P7_LIB} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    # sudo cp libP7.so to /usr/local/lib
    # cd /usr/local/lib
    # sudo ldconfig -n
    exec_program("ln -s ${P7_LIB} ${CMAKE_CURRENT_BINARY_DIR}/libP7.so.2")
    #set(OPENSSL_LIBRARY_PATH "/home/user/projects/library/openssl")
    #set(OPENSSL_INCLUDE_PATH "/home/user/projects/library/openssl/include")
    find_library(SSL_LIB ssl ${OPENSSL_LIBRARY_PATH})
    find_library(CRYPTO_LIB crypto ${OPENSSL_LIBRARY_PATH})
    #set(CPPREST_DIR "/home/user/projects/library/casablanca")
    find_library(CPPREST_LIB NAMES ${CPPREST_LIB_NAME} PATHS ${CPPREST_LIBRARY_DIR})
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH_})
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH_})
endif()

if(NOT DEFINED Boost_FOUND)
	message(FATAL_ERROR "Boost NOT FOUND")
endif()

message(STATUS "Boost_INCLUDE_DIRS - ${Boost_INCLUDE_DIRS}")
message(STATUS "PROJECT_BINARY_DIR - ${PROJECT_BINARY_DIR}")
message(STATUS "CMAKE_BINARY_DIR - ${CMAKE_BINARY_DIR}")
message(STATUS "Boost_LIBRARIES - ${Boost_LIBRARIES}")
message(STATUS "CMAKE_MODULE_PATH - ${CMAKE_MODULE_PATH}")
message(STATUS "CMAKE_INCLUDE_PATH - ${CMAKE_INCLUDE_PATH}")
message(STATUS "RUNTIME - ${RUNTIME}")

add_subdirectory("client")
add_subdirectory("license")
add_subdirectory("parser")
add_subdirectory("tracer")
add_subdirectory("get_license_task")
add_subdirectory("process_killer_task")

if(CMAKE_HOST_WIN32)
    set(SOURCE_LIST main.cpp application.cpp application.h ./tools/constants.cpp ./tools/gason.cpp stdafx.h)
    set(LINK_LIBS tracer NT_Service ${ACE} ${P7_LIB} ${Boost_LIBRARIES} runtimeobject license_checker)
    set(INCLUDE_DIRS client license tracer tools nt_service get_license_task process_killer_task
        ${Boost_INCLUDE_DIRS}
        ${DEFAULT_LIBP7_INCLUDE_DIR})
    add_executable(${PROJECT_NAME} ${SOURCE_LIST})
    target_compile_definitions(${PROJECT_NAME} PUBLIC -D_WIN32_WINNT=0x0601 -D_WINSOCK_DEPRECATED_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS -D_UNICODE -DUNICODE -D_WIN32 -D_WINDOWS)
    add_subdirectory("nt_service")
    target_link_libraries(${PROJECT_NAME} PUBLIC ${LINK_LIBS})
    target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})
elseif(CMAKE_HOST_UNIX)
    set(SOURCES_LIST main.cpp application.cpp application.h ../tools/constants.cpp ../tools/gason.cpp stdafx.h)
    set(LINK_LIBS client license_checker eventsink tracer pthread get_license_task ${CPPREST_LIB} ${Boost_LIBRARIES} ${SSL_LIB} ${CRYPTO_LIB} dl)
    set(INCLUDE_DIRS client license event_sink_linux ../tracer ../tools ../get_license_task ../process_killer_task ../common
        ${Boost_INCLUDE_DIRS}
        ${CPPREST_INCLUDE_DIR}
        ${OPENSSL_INCLUDE_PATH}
        ${DEFAULT_LIBP7_INCLUDE_DIR})
    add_executable(${PROJECT_NAME} ${SOURCES_LIST})
    add_subdirectory("event_sink_linux")
    set(DEPRECATION_FLAG "-Wno-deprecated-declarations")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEPRECATION_FLAG}")
    target_link_libraries(${PROJECT_NAME} PUBLIC ${LINK_LIBS} ${P7_LIB} )
    target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})
endif()

if(CMAKE_HOST_WIN32)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/lic_check_w.ini DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
elseif(CMAKE_HOST_UNIX)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/lic_check_l.ini DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()
