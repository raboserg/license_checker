cmake_minimum_required(VERSION 3.8)

project(http-client)

add_executable(${PROJECT_NAME} main.cpp http_client.cpp http_client.h)

if($ENV{VCPKG_DIR})
    set(VCPKG_DIR $ENV{VCPKG_DIR})
else()
    if(CMAKE_HOST_WIN32)
        #set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
        set(VCPKG_DIR "d:/library/vcpkg/installed/x64-windows")
    elseif(CMAKE_HOST_UNIX)
        set(VCPKG_DIR "~/projects/vcpkg/installed/x64-linux")

        set(ACE_ROOT "~/install/ACE_wrappers/")
        set(ACE_INCLUDE_DIR "${ACE_ROOT}/include")
        set(ACE_LIBRARY_DIR1 "${ACE_ROOT}/lib")

    endif()
    

    if(CMAKE_HOST_WIN32)
      find_library(ACE_LIBRARY NAMES aced HINTS ${ACE_LIBRARY_DIR})
      target_link_libraries(${PROJECT_NAME} ${ACE_LIBRARY})
      target_include_directories(${PROJECT_NAME} PUBLIC ${ACE_INCLUDE_DIR})
    elseif(CMAKE_HOST_UNIX)
      find_library(ACE_LIBRARY NAMES ACE HINTS ${ACE_LIBRARY_DIR1})
      target_link_libraries(${PROJECT_NAME} ${ACE_LIBRARY} pthread)
    endif()
endif()

#LINK_DIRECTORIES(${ACE_LIBRARY_DIR})
