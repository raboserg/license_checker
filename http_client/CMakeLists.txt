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
    endif()
    
    set(ACE_ROOT ${VCPKG_DIR})
    set(ACE_INCLUDE_DIR "${ACE_ROOT}/include")
    set(ACE_LIBRARY_DIR "${ACE_ROOT}/Debug/lib")

    if(CMAKE_HOST_WIN32)
      find_library(ACE_LIBRARY NAMES aced HINTS ${ACE_LIBRARY_DIR})
      target_link_libraries(${PROJECT_NAME} ${ACE_LIBRARY})
      target_include_directories(${PROJECT_NAME} PUBLIC ${ACE_INCLUDE_DIR})
    elseif(CMAKE_HOST_UNIX)
      find_library(ACE_LIBRARY NAMES ACE HINTS "${ACE_ROOT_DIR}/lib")
      target_link_libraries(${PROJECT_NAME} ${ACE_LIBRARY} pthread)
    endif()
endif()

#LINK_DIRECTORIES(${ACE_LIBRARY_DIR})