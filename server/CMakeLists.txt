cmake_minimum_required (VERSION 3.8)

set(HAEDERS connection.hpp
    connection_manager.hpp
    header.hpp
    mime_types.hpp
    reply.hpp
    request.hpp
    request_handler.hpp
    request_parser.hpp
    server.hpp)

set(SOURCES connection.cpp
    connection_manager.cpp
    main.cpp
    mime_types.cpp
    reply.cpp
    request_handler.cpp
    request_parser.cpp
    server.cpp
	${HAEDERS})


#set(Boost_USE_STATIC_LIBS        ON) # only find static libs
#set(Boost_USE_MULTITHREADED      ON)
#set(Boost_USE_STATIC_RUNTIME    ON)
#$add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
#if(CMAKE_GENERATOR_PLATFORM=x64)	message("CMAKE_GENERATOR_PLATFORM=x64")

#set(VCPKG_DIR "d:/library/vcpkg/installed/x64-windows/")
#set(BOOST_ROOT ${VCPKG_DIR})

#find_package(Boost COMPONENTS REQUIRED boost_system-vc140-mt boost_filesystem-vc140-mt boost_program_options-vc140-mt)

#SET(CMAKE_LIBRARY_PATH "${VCPKG_DIR}/lib")
#LINK_DIRECTORIES("${VCPKG_DIR}/lib")

#find_package(Boost REQUIRED)
#??target_compile_definitions(server PUBLIC _WIN32_WINNT)
if(CMAKE_HOST_WIN32)
    add_definitions(-D_WIN32_WINNT=0X0601)
    #???target_compile_definitions(server PUBLIC -D_WIN32_WINNT=0X0601)
endif()
add_library(server STATIC ${SOURCES})

#target_link_libraries(server PUBLIC debug client license boost_system-vc140-mt boost_filesystem-vc140-mt boost_program_options-vc140-mt)
target_include_directories(server PUBLIC ${Boost_INCLUDE_DIRS})
