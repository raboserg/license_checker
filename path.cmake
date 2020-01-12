if(NOT DEFINED $ENV{VCPKG_DIR})
    if(CMAKE_HOST_WIN32)
        set(VCPKG_ROOT "D:/library/vcpkg")
    elseif(CMAKE_HOST_UNIX)
        set(VCPKG_ROOT "~/projects/vcpkg")
    endif()
else()
    set(VCPKG_ROOT $ENV{VCPKG_DIR})
endif()

if(DEFINED VCPKG_TARGET_TRIPLET)
  set(VCPKG_DIR "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}")
elseif(NOT DEFINED VCPKG_TARGET_TRIPLET)
    message(WARNING "need to define VCPKG_TARGET_TRIPLET variabe")
    if(CMAKE_HOST_WIN32)
        set(VCPKG_TARGET_TRIPLET "x64-windows")
    elseif(CMAKE_HOST_UNIX)
        set(VCPKG_TARGET_TRIPLET "x64-linux")
    endif()
        set(VCPKG_DIR "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}")
endif()

message("GDFGDFGFDG ===== ${VCPKG_DIR}")

if(CMAKE_HOST_WIN32)
    if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    endif()
elseif(CMAKE_HOST_UNIX)
    #if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
            set(CMAKE_LIBRARY_PATH "${VCPKG_DIR}/debug/lib")
        else()
            set(CMAKE_LIBRARY_PATH "${VCPKG_DIR}/lib")
        endif()
        set(CMAKE_INCLUDE_PATH  "${VCPKG_DIR}/include")
    #endif()
    set(DEFAULT_WIN_BOOST_DIR "~/projects/vcpkg/installed/x64-linux")
    set(DEFAULT_UNIX_BOOST_DIR "~/projects/library/boost_1_72_0")
endif()
#set(DEFAULT_WIN_LOCAL_DIRECTORY "d:/work/local/")
#set(DEFAULT_UNIX_LOCAL_DIRECTORY "/home/user/local/")
