cmake_minimum_required (VERSION 3.8)

if (UNIX)
  add_definitions(-Wno-sign-compare -Wno-enum-compare)
endif()

if(CMAKE_HOST_WIN32)
    set(CPPREST_DIR D:/library/vcpkg/packages/cpprestsdk_x64-windows)
    set(CPPREST_LIB_NAME "cpprest_2_10d")
	set(CPPREST_ROOT ${CPPREST_DIR})
	set(CPPREST_HEADER_DIR ${CPPREST_ROOT}/include)
	LINK_DIRECTORIES("${CPPREST_ROOT}/debug/lib")
elseif(CMAKE_HOST_UNIX)
    set(CPPREST_DIR ~/projects/casablanca)
    set(CPPREST_LIB_NAME cpprest)
	set(CPPREST_ROOT ${CPPREST_DIR})
	LINK_DIRECTORIES("${CPPREST_ROOT}/build.debug/Release/libs")
endif()

add_library(restserver
    Rest_Server.cpp
    Dealer.cpp
    Table.cpp)
#add_executable(restserver
# Rest_Server.cpp
# Dealer.cpp
 #Table.cpp)

#find_package(cpprestsdk CONFIG REQUIRED)

target_link_libraries(restserver ${CPPREST_LIB_NAME})
target_include_directories(restserver PUBLIC ${CPPREST_HEADER_DIR})
#target_include_directories(restserver PUBLIC cpprestsdk)

###set(cpprestsdk_DIR /usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}/cmake/)
#set(cpprestsdk ${VCPKG_DIR}share/cpprestsdk)
#set(cpprestsdk_DIR ${CPPREST_ROOT})

#set(CPPREST_EXPORT_DIR cpprestsdk CACHE STRING "Directory to install CMake config files.")

#find_package(cpprest REQUIRED ${VCPKG_DIR})


#find_package(cpprestsdk REQUIRED NAMES cpprestsdk cpprest)

#target_link_libraries(restserver cpprest_2_10)
#target_include_directories(restserver PUBLIC ${CPPREST_INCLUDE_DIR})
