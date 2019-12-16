cmake_minimum_required (VERSION 3.8)

project ("license_checker")

set(Boost_USE_STATIC_LIBS    ON)
set(Boost_USE_MULTITHREADED  ON)
set(Boost_USE_STATIC_RUNTIME ON)
#set(Boost_NO_SYSTEM_PATHS    ON)

message("ENV{VCPKG_DIR} - $ENV{VCPKG_DIR}")
message("CMAKE_PREFIX_PATH - ${CMAKE_PREFIX_PATH}")

if($ENV{VCPKG_DIR})
    set(VCPKG_DIR $ENV{VCPKG_DIR})
else()
    if(CMAKE_HOST_WIN32)
        set(VCPKG_DIR "d:/library/vcpkg/installed/x64-windows")
    elseif(CMAKE_HOST_UNIX)
        set(VCPKG_DIR "~/projects/vcpkg/installed/x64-linux")
    endif()
    set(BOOST_ROOT ${VCPKG_DIR})
    set(BOOST_INCLUDE_DIRS "${BOOST_ROOT}/include")
    set(BOOST_LIBRARY_DIRS "${BOOST_ROOT}/lib")
endif()

#if(Boost_NO_SYSTEM_PATHS)
#  set(BOOST_ROOT ${VCPKG_DIR})
#  set(BOOST_INCLUDE_DIRS "${BOOST_ROOT}/include")
#  set(BOOST_LIBRARY_DIRS "${BOOST_ROOT}/lib")
#endif(Boost_NO_SYSTEM_PATHS)

SET(CMAKE_LIBRARY_PATH "${VCPKG_DIR}/lib")
LINK_DIRECTORIES("${VCPKG_DIR}/lib")
message("VCPKG_DIR - ${VCPKG_DIR}")

find_package(Boost REQUIRED)
#???find_library(Boost NAMES Boost HINTS "${BOOST_ROOT}/lib")


#find_package(Boost REQUIRED PATHS D:/library/vcpkg/packages/)
#find_package(Boost REQUIRED boost_system boost_filesystem boost_program_options)

message("Boost_INCLUDE_DIRS - ${Boost_INCLUDE_DIRS}")
message("PROJECT_BINARY_DIR - ${PROJECT_BINARY_DIR}")
message("CMAKE_BINARY_DIR - ${CMAKE_BINARY_DIR}")
message("Boost_LIBRARIES - ${Boost_LIBRARIES}")
message("CMAKE_MODULE_PATH - ${CMAKE_MODULE_PATH}")

if(Boost_FOUND)
        #???add_definitions(-D_WIN32_WINNT=0X0601)
	add_executable(${PROJECT_NAME} application.cpp application.h)
	#vc140-mt
        if(CMAKE_HOST_WIN32)
			#set(Boost_INCLUDE_DIRS "${BOOST_ROOT}/include")
            add_definitions(-D_WIN32_WINNT=0X0601)
            #???target_compile_definitions(${PROJECT_NAME} PUBLIC -D_WIN32_WINNT=0X0601)
            target_link_libraries(${PROJECT_NAME} PUBLIC client license server
                boost_system-vc140-mt
                boost_filesystem-vc140-mt
                boost_program_options-vc140-mt)
        elseif(CMAKE_HOST_UNIX)
            target_link_libraries(${PROJECT_NAME} PUBLIC client license server
                pthread
                boost_system
                boost_filesystem
                boost_program_options)
        endif()
	#????add_library(${PROJECT_NAME} STATIC license boost_system-vc140-mt boost_filesystem-vc140-mt boost_program_options-vc140-mt)
    #target_include_directories(${PROJECT_NAME} PUBLIC client license http_server server ${Boost_INCLUDE_DIRS})
    target_include_directories(${PROJECT_NAME} PUBLIC
            client
            license
            http_server
            server
            ${Boost_INCLUDE_DIRS})
    add_subdirectory ("client")
	add_subdirectory ("license")
	add_subdirectory("server")
	add_subdirectory("cpprest")
	add_subdirectory("http_server")
  add_subdirectory("template")

    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/itvpn.ini DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
	if(CMAKE_HOST_WIN32)
		#vc142-mt-x64-1_71.dll
		set(DEPENDENCIES ${BOOST_ROOT}/bin/boost_system-vc142-mt-x64-1_71.dll
				${BOOST_ROOT}/bin/boost_filesystem-vc142-mt-x64-1_71.dll
				${BOOST_ROOT}/bin/boost_program_options-vc142-mt-x64-1_71.dll)
		foreach(DEPENDENCY ${DEPENDENCIES})
                    file(COPY ${DEPENDENCY} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
                    #???install(PROGRAMS "${DEPENDENCY}" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
		endforeach()
	elseif(CMAKE_HOST_UNIX)
		message("CMAKE_HOST_UNIX")
	endif()

endif()
