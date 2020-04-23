cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 14)

if(NOT DEFINED ${CMAKE_BUILD_TYPE})
        #set(CMAKE_BUILD_TYPE "Debug")
        set(CMAKE_BUILD_TYPE "Ralease")
endif()

include(path.cmake)

if(CMAKE_HOST_WIN32)
    project ("itVPNAgent")
elseif(CMAKE_HOST_UNIX)
    project ("itvpnagent")
#    set(CMAKE_C_COMPILER "/usr/bin/gcc-5")
#    set(CMAKE_CXX_COMPILER "/usr/bin/g++-5")
endif()

set(VS_STARTUP_PROJECT ${PROJECT_NAME})

#set(LINK "STATIC")
set(LINK "SHARED")

#set(VARIANT "RELEASE")
set(VARIANT "DEBUG")

set(ROOT_PROJECT_BINARY_DIR ${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE})

set(CMAKE_USER_MAKE_RULES_OVERRIDE "build.cmake")
include(build.cmake)

message(STATUS "$$$$$ ROOT_PROJECT_BINARY_DIR - ${ROOT_PROJECT_BINARY_DIR}")
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
        set(CPPREST_LIB_NAME "cpprest")
        set(P7_LIB_NAME "P7x64.dll")
        set(ACE_LIB_NAME "ACE")
    endif()
    if (VCPKG_TARGET_TRIPLET STREQUAL "x86-windows")
        if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
            set(P7_LIB_NAME "P7x32d")
        else()
            set(P7_LIB_NAME "P7x32")
        endif()
    endif()
elseif(CMAKE_HOST_UNIX)
    set(CPPREST_LIB_NAME cpprest)
    set(ACE_LIB_NAME "ACE")
    set(P7_LIB_NAME "P7")
endif()

if(CMAKE_HOST_WIN32)
#    set(cpprestsdk_DIR "${VCPKG_ROOT}/installed/x64-windows/share/cpprestsdk")
#	set(unofficial-brotli_DIR "${VCPKG_ROOT}/installed/x64-windows/share/unofficial-brotli")
#	find_package(cpprestsdk REQUIRED)
#	message("cpprestsdk_INCLUDE_DIR ${cpprestsdk_INCLUDE_DIR}")
#	message("cpprestsdk_LIBRARIES ${cpprestsdk_LIBRARIES}")

#    find_package(Boost 1.71.0 COMPONENTS filesystem REQUIRED)
    find_package(Boost COMPONENTS filesystem)
    find_library(P7_LIB NAMES ${P7_LIB_NAME} HINTS ${DEFAULT_LIBP7_LIB_DIR})
    find_library(CPPREST_LIB NAMES ${CPPREST_LIB_NAME} HINTS ${VCPKG_DIR}/lib)
    find_library(ACE NAMES ${ACE_LIB_NAME} HINTS ${ACE_LIBRARY_DIR})
	#find_library(LICENSE license HINTS "D:/library/vcpkg/installed/x64-windows-static/debug/lib")
elseif(CMAKE_HOST_UNIX)
    find_library(ACE NAMES ${ACE_LIB_NAME} PATHS ${ACE_LIBRARY_DIR})
    find_package(Boost COMPONENTS system filesystem)
#    set(CMAKE_LIBRARY_PATH ${DEFAULT_UNIX_LIBP7_DIR}/Binaries)
    find_library(P7_LIB NAMES ${P7_LIB_NAME} PATHS ${DEFAULT_UNIX_LIBP7_DIR})
#    file(COPY ${P7_LIB} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    # sudo cp libP7.so to /usr/local/lib
    # cd /usr/local/lib
    # sudo ldconfig -n
#    exec_program("ln -s ${P7_LIB} ${CMAKE_CURRENT_BINARY_DIR}/libP7.so.2")
    find_library(SSL_LIB ssl ${OPENSSL_LIBRARY_PATH})
    find_library(CRYPTO_LIB crypto ${OPENSSL_LIBRARY_PATH})

    find_library(CPPREST_LIB NAMES ${CPPREST_LIB_NAME} PATHS ${CPPREST_LIBRARY_DIR})
#    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH_})
#    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH_})
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
add_subdirectory("tools")
add_subdirectory("license")
add_subdirectory("parser")
add_subdirectory("tracer")
add_subdirectory("constants")
add_subdirectory("event_sink_task")
add_subdirectory("get_license_task")
add_subdirectory("process_killer_task")

if(CMAKE_HOST_WIN32)
    set(SOURCE_LIST main.cpp application.cpp application.h stdafx.h)
    set(LINK_LIBS NT_Service)
    set(INCLUDE_DIRS nt_service constants ${DEFAULT_LIBP7_INCLUDE_DIR})
    add_executable(${PROJECT_NAME} ${SOURCE_LIST})
    add_subdirectory("nt_service")
    target_link_libraries(${PROJECT_NAME} PUBLIC ${LINK_LIBS})
    target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
	if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		target_compile_definitions(${PROJECT_NAME} PUBLIC -DCONNEC_CONFIG)
	endif()
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DCONNEC_CONFIG -D_WIN32_WINNT
        -D_WIN32_WINNT=0x0601 -D_WINSOCK_DEPRECATED_NO_WARNINGS
        -D_CRT_SECURE_NO_WARNINGS -D_UNICODE -DUNICODE -D_WIN32 -D_WINDOWS)
elseif(CMAKE_HOST_UNIX)
    set(SOURCES_LIST main.cpp application.cpp application.h stdafx.h)
    set(LINK_LIBS nx_service ${CRYPTO_LIB})
    set(INCLUDE_DIRS client license constants nx_service tracer tools parser
        get_license_task process_killer_task common
        ${Boost_INCLUDE_DIRS}
        ${CPPREST_INCLUDE_DIR}
        ${OPENSSL_INCLUDE_PATH}
        ${ACE_INCLUDE_DIR}
        ${DEFAULT_LIBP7_INCLUDE_DIR})
    add_executable(${PROJECT_NAME} ${SOURCES_LIST})
    add_subdirectory("nx_service")
    set(DEPRECATION_FLAG "-Wno-deprecated-declarations")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEPRECATION_FLAG}")
    add_definitions(-D_DEBUG)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${LINK_LIBS})
    target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
#    install(TARGETS ${ACE} LIBRARY
#        DESTINATION bin
#        COMPONENT Libraries
#        NAMELINK_COMPONENT Development)
#    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
#    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
#                 "Debug"
#                 "Release"
#                 "MinSizeRel"
#                 "RelWithDebInfo")

endif()

if(CMAKE_HOST_WIN32)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/itvpnagent.ini DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
elseif(CMAKE_HOST_UNIX)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/lic_check_l.ini DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()
