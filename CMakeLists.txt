cmake_minimum_required (VERSION 3.8)
include(path.cmake)
project ("license_checker")

set(LINK "STATIC")
set(VARIANT "DEBUG")
include(build.cmake)

message(STATUS "$$$$$ CMAKE_CROSSCOMPILING - ${CMAKE_CROSSCOMPILING}")
message(STATUS "$$$$$ CMAKE_INCLUDE_PATH - ${CMAKE_INCLUDE_PATH}")
message(STATUS "$$$$$ CMAKE_LIBRARY_PATH - ${CMAKE_LIBRARY_PATH}")

#set(Boost_NO_SYSTEM_PATHS    ON)
#set(Boost_USE_STATIC_LIBS    OFF)  # only find static libs
set(Boost_USE_DEBUG_LIBS      ON)
#set(Boost_USE_RELEASE_LIBS   OFF)  # only find release libs
#set(Boost_USE_MULTITHREADED   ON)
#set(Boost_USE_STATIC_RUNTIME OFF)

if(CMAKE_HOST_UNIX)
    set(CMAKE_LIBRARY_PATH_OLD ${CMAKE_LIBRARY_PATH})
    set(CMAKE_INCLUDE_PATH_OLD ${CMAKE_INCLUDE_PATH})
    set(CMAKE_LIBRARY_PATH ${DEFAULT_UNIX_BOOST_DIR}/stage/lib)
    set(CMAKE_INCLUDE_PATH ${DEFAULT_UNIX_BOOST_DIR})
#    set(BOOST_INCLUDEDIR ${DEFAULT_UNIX_BOOST_DIR})
#    set(BOOST_LIBRARYDIR ${DEFAULT_UNIX_BOOST_DIR}/stage/lib)
    find_package(Boost COMPONENTS system)
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH_OLD})
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH_OLD})
    find_library(SSL_LIB NAMES ssl PATHS ${CMAKE_LIBRARY_PATH})
    find_library(CRYPTO_LIB NAMES crypto PATHS ${CMAKE_LIBRARY_PATH})
else()
    find_package(Boost 1.71.0 COMPONENTS system filesystem REQUIRED)
endif()

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
        target_include_directories(${PROJECT_NAME} PRIVTE client license event_sink_win ${Boost_INCLUDE_DIRS})
    elseif(CMAKE_HOST_UNIX)
        add_subdirectory("event_sink_linux")
        target_link_libraries(${PROJECT_NAME} PUBLIC
            client
            license
            eventsink
            pthread
            ${Boost_LIBRARIES} ${SSL_LIB} ${CRYPTO_LIB} dl)
        target_include_directories(${PROJECT_NAME} PRIVATE client license event_sink_linux ${Boost_INCLUDE_DIRS})
    endif()
    add_subdirectory("client")
    add_subdirectory("license")
    add_subdirectory("parser")
    #set(BOOST_ROOT ${CMAKE_PREFIX_PATH})
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
