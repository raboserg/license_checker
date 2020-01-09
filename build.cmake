set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(BUILD_TYPE_LIST "DEBUG;RELEASE")
set(CMAKE_CONFIGURATION_TYPES ${BUILD_TYPE_LIST} CACHE STRING "Configurations" FORCE)

#set(CMAKE_BUILD_TYPE ${VARIANT})
#if(CMAKE_BUILD_TYPE)
#  set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}
#      CACHE STRING "Choose the type of build: DEBUG RELEASE"      FORCE)
#else()  set(CMAKE_BUILD_TYPE RELEASE      CACHE STRING "Choose the type of build: DEBUG RELEASE"      FORCE)endif()

include(path.cmake)

if($ENV{TOOLCHAIN_FILE})
	message(STATUS "$ENV{TOOLCHAIN_FILE} - $ENV{TOOLCHAIN_FILE}")
	set(CMAKE_TOOLCHAIN_FILE $ENV{TOOLCHAIN_FILE})
else()
	if(CMAKE_HOST_WIN32)
		set(CMAKE_TOOLCHAIN_FILE ${DEFAULT_WIN_TOOLCHAIN_FILE})
	elseif(CMAKE_HOST_UNIX)
		set(CMAKE_TOOLCHAIN_FILE ${DEFAULT_UNIX_TOOLCHAIN_FILE})
	endif()
	message(STATUS "CMAKE_TOOLCHAIN_FILE - ${CMAKE_TOOLCHAIN_FILE}")
endif()

if($ENV{VCPKG_DIR})
    set(VCPKG_DIR $ENV{VCPKG_DIR})
else()
    if(CMAKE_HOST_WIN32)
        set(VCPKG_DIR ${DEFAULT_WIN_VCPKG_DIR_DIR})
    elseif(CMAKE_HOST_UNIX)
        set(VCPKG_DIR ${DEFAULT_UNIX_VCPKG_DIR_DIR})
    endif()
endif()

message(STATUS "ENV{VCPKG_DIR} - $ENV{VCPKG_DIR}")
message(STATUS "CMAKE_PREFIX_PATH - ${CMAKE_PREFIX_PATH}")

set(LOCAL_DIRECTORY "${LOCAL_DIRECTORY}")
set(THIRDPARTY_DIRECTORY "${THIRDPARTY_DIRECTORY}")

message(STATUS "CMAKE_CXX_COMPILER_ID: [${CMAKE_CXX_COMPILER_ID}]")

if(WIN32)
	set(OS "windows")
elseif(UNIX)
	set(OS "linux")
else()
	message(ERROR "UNKNOW OPERATION SYSTEM!")
endif()

message(STATUS "CMAKE_LIBRARY_ARCHITECTURE_REGEX: [${CMAKE_LIBRARY_ARCHITECTURE_REGEX}]")
message(STATUS "CMAKE_LIBRARY_ARCHITECTURE: [${CMAKE_LIBRARY_ARCHITECTURE}]")
message(STATUS "CMAKE_SYSTEM_PROCESSOR: [${CMAKE_SYSTEM_PROCESSOR}]")
string(FIND "${CMAKE_SYSTEM_PROCESSOR}" "arm" ARCH)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    if (LOCAL_DIRECTORY STREQUAL "")
        set(LOCAL_DIRECTORY ${DEFAULT_WIN_LOCAL_DIRECTORY})
    endif()
    if (THIRDPARTY_DIRECTORY STREQUAL "")
        set(THIRDPARTY_DIRECTORY ${DEFAULT_WIN_THIRDPARTY_DIRECTORY})
    endif()

    if(${CMAKE_CL_64})		
        set(ARCH "x64")
        set(COMPILER "${CMAKE_CXX_COMPILER_ID}")
    else()
        set(ARCH "x86")
        set(COMPILER "${CMAKE_CXX_COMPILER_ID}")
    endif() 
    
    if(${MSVC_VERSION} EQUAL 1800)
        set(COMPILER_VERSION "12")
    elseif(${MSVC_VERSION} EQUAL 1900) 
        set(COMPILER_VERSION "14")
    elseif(${MSVC_VERSION} EQUAL 1910) 
        set(COMPILER_VERSION "15")
    elseif(${MSVC_VERSION} EQUAL 1911)
        set(COMPILER_VERSION "17")
    endif()
endif()

if(CMAKE_COMPILER_IS_GNUCC)
    if (LOCAL_DIRECTORY STREQUAL "")
        if(WIN32)
            set(LOCAL_DIRECTORY ${DEFAULT_WIN_LOCAL_DIRECTORY})
        else()
            set(LOCAL_DIRECTORY ${DEFAULT_UNIX_LOCAL_DIRECTORY})
        endif()
    endif()
    if (THIRDPARTY_DIRECTORY STREQUAL "")
        if(WIN32)
            set(THIRDPARTY_DIRECTORY ${DEFAULT_WIN_THIRDPARTY_DIRECTORY})
        else()
            set(THIRDPARTY_DIRECTORY ${DEFAULT_UNIX_THIRDPARTY_DIRECTORY})
        endif()
    endif()
	
	set(COMPILER "gcc")
	if(WIN32)			
	else()
            if(NOT ${ARCH} EQUAL -1)
                set(ARCH "${CMAKE_SYSTEM_PROCESSOR}")
            elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
                set(ARCH "amd64")
            else()
                set(ARCH "i386")
            endif()
            set(COMPILER_VERSION "${CMAKE_C_COMPILER_VERSION}")
	endif()
endif()

if (CMAKE_CXX_COMPILER MATCHES ".*clang")
    set(CMAKE_COMPILER_IS_CLANGXX 1)
endif ()

if(CMAKE_COMPILER_IS_CLANGXX)
    set(COMPILER "clang")
endif()

message(STATUS "ENV=${ENV}")
#TODO make new file for this
#???string(SUBSTRING ${VARIANT} 0 1 VARIANT_STRING)
#???string(TOLOWER ${VARIANT_STRING} VARIANT_STRING)

message(STATUS "OS: ${OS}")
message(STATUS "ARCH: [${ARCH}]")
message(STATUS "COMPILER: [${COMPILER}]")
message(STATUS "COMPILER_VERSION: [${COMPILER_VERSION}]")
string(TOLOWER ${COMPILER} COMPILER)

#BUILD FLAGS
option(BUILD_TESTS "Build tests." OFF)
option(BUILD_DOC "Build documentation (doxygen required)." OFF)
option(BUILD_SHARED_LIBS "DLL" OFF)
option(BUILD_STATIC_LIBS  "LIB" OFF)

if(${LINK} STREQUAL "STATIC")
    set(BUILD_STATIC_LIBS ON)
    set(BUILD_SHARED_LIBS OFF)
endif()
if(${LINK} STREQUAL "SHARED")
    set(BUILD_STATIC_LIBS OFF)
    set(BUILD_SHARED_LIBS ON)
endif()

message(STATUS "CMAKE_GENERATOR: [${CMAKE_GENERATOR}]")
message(STATUS "LINK: [${LINK}]")
message(STATUS "VARIANT: [${VARIANT}]")
message(STATUS "RUNTIME: [${RUNTIME}]")

#add BUILD FLAGS to compiler flags
if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    set(CMAKE_C_FLAGS_RELEASE     "/${RUNTIME} ${CMAKE_C_FLAGS_RELEASE} -D_UNICODE -DUNICODE")  
    set(CMAKE_C_FLAGS_DEBUG       "/${RUNTIME}d ${CMAKE_C_FLAGS_DEBUG} -D_UNICODE -DUNICODE")
    set(CMAKE_C_FLAGS_COVERAGE    "-O0 -g3 -coverage")
    set(CMAKE_C_FLAGS_ASAN        "-Werror -fsanitize=address -fno-common -O3")
    set(CMAKE_C_FLAGS_ASANDBG     "-Werror -fsanitize=address -fno-common -O1 -g3 -fno-omit-frame-pointer -fno-optimize-sibling-calls ")
    set(CMAKE_C_FLAGS_CHECK       "-Werror -O1 -Wcast-qual")

    set(CMAKE_CXX_FLAGS_RELEASE     "/${RUNTIME} ${CMAKE_CXX_FLAGS_RELEASE} -D_UNICODE -DUNICODE")  
    set(CMAKE_CXX_FLAGS_DEBUG       "/${RUNTIME}d ${CMAKE_CXX_FLAGS_DEBUG} -D_UNICODE -DUNICODE")
    set(CMAKE_CXX_FLAGS_COVERAGE    "-O0 -g3 -coverage")
    set(CMAKE_CXX_FLAGS_ASAN        "-Werror -fsanitize=address -fno-common -O3")
    set(CMAKE_CXX_FLAGS_ASANDBG     "-Werror -fsanitize=address -fno-common -O1 -g3 -fno-omit-frame-pointer -fno-optimize-sibling-calls ")
    set(CMAKE_CXX_FLAGS_CHECK       "-Werror -O1 -Wcast-qual")
endif()
message(STATUS "CMAKE_CXX_FLAGS_DEBUG:[${CMAKE_CXX_FLAGS_DEBUG}]")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE:[${CMAKE_CXX_FLAGS_RELEASE}]")

if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS_RELEASE     "${CMAKE_C_FLAGS_RELEASE} -O2 -D_UNICODE -DUNICODE")
    set(CMAKE_C_FLAGS_DEBUG       "${CMAKE_C_FLAGS_DEBUG} -O0 -g3 -D_UNICODE -DUNICODE")
    set(CMAKE_C_FLAGS_COVERAGE    "-O0 -g3 -coverage")
    set(CMAKE_C_FLAGS_ASAN        "-Werror -fsanitize=address -fno-common -O3")
    set(CMAKE_C_FLAGS_ASANDBG     "-Werror -fsanitize=address -fno-common -O1 -g3 -fno-omit-frame-pointer -fno-optimize-sibling-calls ")
    set(CMAKE_C_FLAGS_CHECK       "-Werror -O1 -Wcast-qual")
    
    set(CMAKE_CXX_FLAGS_RELEASE     "${CMAKE_CXX_FLAGS_RELEASE} -O2 -D_UNICODE -DUNICODE")
    set(CMAKE_CXX_FLAGS_DEBUG       "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3 -D_UNICODE -DUNICODE")
    set(CMAKE_CXX_FLAGS_COVERAGE    "-O0 -g3 -coverage")
    set(CMAKE_CXX_FLAGS_ASAN        "-Werror -fsanitize=address -fno-common -O3")
    set(CMAKE_CXX_FLAGS_ASANDBG     "-Werror -fsanitize=address -fno-common -O1 -g3 -fno-omit-frame-pointer -fno-optimize-sibling-calls ")
    set(CMAKE_CXX_FLAGS_CHECK       "-Werror -O1 -Wcast-qual")
endif(CMAKE_COMPILER_IS_GNUCC)

if(CMAKE_COMPILER_IS_CLANGXX)
    set(CMAKE_C_FLAGS_RELEASE     "${CMAKE_C_FLAGS_RELEASE} -O2 -D_UNICODE -DUNICODE")
    set(CMAKE_C_FLAGS_DEBUG       "${CMAKE_C_FLAGS_DEBUG} -O0 -g3 -D_UNICODE -DUNICODE")
    set(CMAKE_C_FLAGS_COVERAGE    "-O0 -g3 -coverage")
    set(CMAKE_C_FLAGS_ASAN        "-Werror -fsanitize=address -fno-common -O3")
    set(CMAKE_C_FLAGS_ASANDBG     "-Werror -fsanitize=address -fno-common -O1 -g3 -fno-omit-frame-pointer -fno-optimize-sibling-calls ")
    set(CMAKE_C_FLAGS_CHECK       "-Werror -O1 -Wcast-qual")

    set(CMAKE_CXX_FLAGS_RELEASE     "${CMAKE_CXX_FLAGS_RELEASE} -O2 -D_UNICODE -DUNICODE")
    set(CMAKE_CXX_FLAGS_DEBUG       "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3 -D_UNICODE -DUNICODE")
    set(CMAKE_CXX_FLAGS_COVERAGE    "-O0 -g3 -coverage")
    set(CMAKE_CXX_FLAGS_ASAN        "-Werror -fsanitize=address -fno-common -O3")
    set(CMAKE_CXX_FLAGS_ASANDBG     "-Werror -fsanitize=address -fno-common -O1 -g3 -fno-omit-frame-pointer -fno-optimize-sibling-calls ")
    set(CMAKE_CXX_FLAGS_CHECK       "-Werror -O1 -Wcast-qual")
endif(CMAKE_COMPILER_IS_CLANGXX)

if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
    if(CMAKE_COMPILER_IS_GNUCC)
        set(CMAKE_SHARED_LINKER_FLAGS "-fprofile-arcs -ftest-coverage")
    endif(CMAKE_COMPILER_IS_GNUCC)
endif(CMAKE_BUILD_TYPE STREQUAL "Coverage")

message(STATUS "CMAKE_BUILD_TYPE:[${CMAKE_BUILD_TYPE}]")

#????????????????if (UNIX)
#  add_definitions(-Wno-sign-compare -Wno-enum-compare)
#endif()

function(set_inc_dir _PROJECT)
    set(${_PROJECT}_INCLUDE_DIR ${LOCAL_DIRECTORY}${_PROJECT}/${${_PROJECT}_VERSION}/include/ PARENT_SCOPE)
endfunction()
function(set_lib_dir _PROJECT)
    set(${_PROJECT}_LIBRARY_DIR ${LOCAL_DIRECTORY}${_PROJECT}/${${_PROJECT}_VERSION}/lib/${OS}-${ARCH}-${COMPILER}-${COMPILER_VERSION}-${VARIANT_STRING}/ PARENT_SCOPE)
endfunction()
function(set_bin_dir _PROJECT)
    set(${_PROJECT}_BINARY_DIR ${LOCAL_DIRECTORY}${_PROJECT}/${${_PROJECT}_VERSION}/app/${OS}-${ARCH}-${COMPILER}-${COMPILER_VERSION}-${VARIANT_STRING}/ PARENT_SCOPE)
endfunction()

function(inc_dirs _PROJECT)
    set_inc_dir(${_PROJECT})
    #message( "inc_dir(${_PROJECT}_INCLUDE_DIR) = [${${_PROJECT}_INCLUDE_DIR}]" )
    include_directories(${${_PROJECT}_INCLUDE_DIR})
endfunction()
function(lib_dirs _PROJECT)
    set_lib_dir(${_PROJECT})
    #message( "lib_dir(${_PROJECT}_LIBRARY_DIR) = [${${_PROJECT}_LIBRARY_DIR}]" )
    link_directories(${${_PROJECT}_LIBRARY_DIR})
endfunction()
