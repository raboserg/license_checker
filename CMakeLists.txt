cmake_minimum_required (VERSION 3.8)

project ("license_checker")

set(VCPKG_TARGET_TRIPLET "x64-windows")
set(LINK "STATIC")
set(VARIANT "DEBUG")

include(build.cmake)

set(Boost_USE_STATIC_LIBS    ON)
set(Boost_USE_MULTITHREADED  ON)
set(Boost_USE_STATIC_RUNTIME ON)
#set(Boost_NO_SYSTEM_PATHS    ON)

message(STATUS "$$$$$ CMAKE_LIBRARY_PATH - ${CMAKE_LIBRARY_PATH}")
message(STATUS "$$$$$ CMAKE_CROSSCOMPILING - ${CMAKE_CROSSCOMPILING}")
message(STATUS "$$$$$ CMAKE_INCLUDE_PATH - ${CMAKE_INCLUDE_PATH}")
message(STATUS "$$$$$ CMAKE_LIBRARY_PATH - ${CMAKE_LIBRARY_PATH}")

find_package(Boost 1.71.0 COMPONENTS system filesystem REQUIRED )
#???find_library(BOOST_LIBS Boost NAMES system filesystem ${VCPKG_DIR}/debug/lib)
#???message(STATUS "&&&&&&& BOOST_LIBS - ${BOOST_LIBS}")

message(STATUS "Boost_INCLUDE_DIRS - ${Boost_INCLUDE_DIRS}")
message(STATUS "PROJECT_BINARY_DIR - ${PROJECT_BINARY_DIR}")
message(STATUS "CMAKE_BINARY_DIR - ${CMAKE_BINARY_DIR}")
message(STATUS "Boost_LIBRARIES - ${Boost_LIBRARIES}")
message(STATUS "CMAKE_MODULE_PATH - ${CMAKE_MODULE_PATH}")
message(STATUS "RUNTIME - ${RUNTIME}")

if(Boost_FOUND) #???
	add_executable(${PROJECT_NAME} application.cpp application.h stdafx.h tools.h)
	#vc140-mt
        if(CMAKE_HOST_WIN32)
            add_subdirectory("event_sink_win")
            target_link_libraries(${PROJECT_NAME} PUBLIC 
                client
                license
                eventsink
                ${Boost_LIBRARIES})
                target_include_directories(${PROJECT_NAME} PUBLIC client license event_sink_win ${Boost_INCLUDE_DIRS})
        elseif(CMAKE_HOST_UNIX)
            add_subdirectory("event_sink_linux")
            target_link_libraries(${PROJECT_NAME} PUBLIC 
                client
                license
                cpprest
                eventsink
                pthread
				${Boost_LIBRARIES} ssl crypto dl)
                #boost_system
                #boost_filesystem
                #boost_program_options
                #ssl crypto dl)
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

	set(BOOST_ROOT ${CMAKE_PREFIX_PATH})

    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/itvpn.ini DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
	if(CMAKE_HOST_WIN32)
		#vc142-mt-x64-1_71.dll
		#set(DEPENDENCIES ${BOOST_ROOT}/Debug/bin/boost_system-vc142-mt-gd-x64-1_71.dll				${BOOST_ROOT}/Debug/bin/boost_filesystem-vc142-mt-gd-x64-1_71.dll				${BOOST_ROOT}/Debug/bin/boost_program_options-vc142-mt-gd-x64-1_71.dll)
		#foreach(DEPENDENCY ${DEPENDENCIES})                    file(COPY ${DEPENDENCY} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
                    #???install(PROGRAMS "${DEPENDENCY}" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
		#endforeach()
	elseif(CMAKE_HOST_UNIX)
		message(STATUS  "IT'S FOR CMAKE_HOST_UNIX")
	endif()

endif()
