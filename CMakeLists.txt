cmake_minimum_required (VERSION 3.8)

project ("license_checker")

set(LINK "STATIC")
set(VARIANT "DEBUG")

include(build.cmake)

set(BOOST_ROOT ${VCPKG_DIR})
set(Boost_USE_STATIC_LIBS    ON)
set(Boost_USE_MULTITHREADED  ON)
set(Boost_USE_STATIC_RUNTIME ON)
#set(Boost_NO_SYSTEM_PATHS    ON)

#set(BOOST_INCLUDE_DIRS "${BOOST_ROOT}/include")
#set(BOOST_LIBRARY_DIRS "${BOOST_ROOT}/debug/lib")
#if(Boost_NO_SYSTEM_PATHS)
#  set(BOOST_ROOT ${VCPKG_DIR})
#  set(BOOST_INCLUDE_DIRS "${BOOST_ROOT}/include")
#  set(BOOST_LIBRARY_DIRS "${BOOST_ROOT}/lib")
#endif(Boost_NO_SYSTEM_PATHS)

SET(CMAKE_LIBRARY_PATH "${VCPKG_DIR}/debug/lib")
LINK_DIRECTORIES("${VCPKG_DIR}/debug/lib")
#LINK_DIRECTORIES("${VCPKG_DIR}/debug/lib" "d:/library/vcpkg/installed/x64-windows-static/debug/lib")
message(STATUS "VCPKG_DIR - ${VCPKG_DIR}")

find_package(Boost REQUIRED)

message(STATUS "Boost_INCLUDE_DIRS - ${Boost_INCLUDE_DIRS}")
message(STATUS "PROJECT_BINARY_DIR - ${PROJECT_BINARY_DIR}")
message(STATUS "CMAKE_BINARY_DIR - ${CMAKE_BINARY_DIR}")
message(STATUS "Boost_LIBRARIES - ${Boost_LIBRARIES}")
message(STATUS "CMAKE_MODULE_PATH - ${CMAKE_MODULE_PATH}")
message(STATUS "RUNTIME - ${RUNTIME}")

if(Boost_FOUND) #???
        #???add_definitions(-D_WIN32_WINNT=0X0601)
	add_executable(${PROJECT_NAME} application.cpp application.h)
	#vc140-mt
        if(CMAKE_HOST_WIN32)
            add_subdirectory("event_sink_win")
            add_definitions(-D_UNICODE -DUNICODE)
			#add_definitions(-D_WIN32_WINNT=0X0601 -D_UNICODE -DUNICODE)
            #???target_compile_definitions(${PROJECT_NAME} PUBLIC -D_WIN32_WINNT=0X0601)
            target_link_libraries(${PROJECT_NAME} PUBLIC 
                client
                license
                cpprest_2_10d
                eventsink
                boost_system-vc140-mt-gd
                boost_filesystem-vc140-mt-gd
                boost_program_options-vc140-mt-gd)
                target_include_directories(${PROJECT_NAME} PUBLIC client license event_sink_win ${Boost_INCLUDE_DIRS})
        elseif(CMAKE_HOST_UNIX)
            add_subdirectory("event_sink_linux")
            target_link_libraries(${PROJECT_NAME} PUBLIC 
                client
                license
                cpprest
                eventsink
                pthread
                boost_system
                boost_filesystem
                boost_program_options
                ssl crypto dl)
            target_include_directories(${PROJECT_NAME} PUBLIC client license event_sink_linux ${Boost_INCLUDE_DIRS})
        endif()
	
    add_subdirectory("client")
    add_subdirectory("license")
    #add_subdirectory("server")
    add_subdirectory("cpprest")
    #add_subdirectory("http_server")
    add_subdirectory("template")
    add_subdirectory("parser")
    #add_subdirectory("http_client")

    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/itvpn.ini DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
	if(CMAKE_HOST_WIN32)
		#vc142-mt-x64-1_71.dll
		set(DEPENDENCIES ${BOOST_ROOT}/Debug/bin/boost_system-vc142-mt-gd-x64-1_71.dll
				${BOOST_ROOT}/Debug/bin/boost_filesystem-vc142-mt-gd-x64-1_71.dll
				${BOOST_ROOT}/Debug/bin/boost_program_options-vc142-mt-gd-x64-1_71.dll)
		foreach(DEPENDENCY ${DEPENDENCIES})
                    file(COPY ${DEPENDENCY} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
                    #???install(PROGRAMS "${DEPENDENCY}" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
		endforeach()
	elseif(CMAKE_HOST_UNIX)
		message(STATUS  "IT'S FOR CMAKE_HOST_UNIX")
	endif()

endif()
