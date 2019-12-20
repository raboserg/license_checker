cmake_minimum_required (VERSION 3.8)

set(SOURCE_LIB client_license.cpp client_license.h)

if (UNIX)
  add_definitions(-Wno-sign-compare -Wno-enum-compare)
endif()

if(CMAKE_HOST_WIN32)
    set(CPPREST_DIR D:/library/vcpkg/packages/cpprestsdk_x64-windows)
    set(CPPREST_LIB_NAME "cpprest_2_10d")
    LINK_DIRECTORIES("${CPPREST_DIR}/debug/lib")
elseif(CMAKE_HOST_UNIX)
    set(CPPREST_DIR ~/projects/vcpkg/packages/cpprestsdk_x64-linux)
    set(CPPREST_LIB_NAME cpprest)
    LINK_DIRECTORIES("${CPPREST_DIR}/build.debug/Release/libs")
endif()

#add_executable(client ${SOURCE_LIB})
add_library (client STATIC ${SOURCE_LIB})

target_include_directories(client PUBLIC ${CPPREST_DIR}/include ${Boost_INCLUDE_DIRS})
#find_package(cpprestsdk CONFIG REQUIRED)
if(CMAKE_HOST_WIN32)
        target_link_libraries(client ${CPPREST_LIB_NAME})
elseif(CMAKE_HOST_UNIX)
        target_link_libraries(client ${CPPREST_LIB_NAME} pthread boost_system crypto ssl pthread dl)
        #target_link_libraries(client ${CPPREST_LIB_NAME})
endif()
#target_link_libraries(restserver pthread ${CPPREST_LIB_NAME})



#target_include_directories(client PUBLIC ${Boost_INCLUDE_DIRS})
