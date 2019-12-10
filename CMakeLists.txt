cmake_minimum_required (VERSION 3.8)

project ("license_checker")

set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     ON)
#$add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
#if(CMAKE_GENERATOR_PLATFORM=x64)	message("CMAKE_GENERATOR_PLATFORM=x64")

set(VCPKG_DIR "d:/library/vcpkg/installed/x64-windows/")
set(BOOST_ROOT ${VCPKG_DIR})

#???find_package(Boost COMPONENTS REQUIRED boost_system boost_filesystem boost_program_options)

SET(CMAKE_LIBRARY_PATH "${VCPKG_DIR}/lib")
LINK_DIRECTORIES("${VCPKG_DIR}/lib")
find_package(Boost REQUIRED)

if(CMAKE_HOST_WIN32)
	message("Boost_INCLUDE_DIRS - ${PROJECT_BINARY_DIR}")
endif()

if(Boost_FOUND)
        #???add_definitions(-D_WIN32_WINNT=0X0601)
	add_executable(${PROJECT_NAME} application.cpp application.h)
	#vc140-mt
        if(CMAKE_HOST_WIN32)
            #???add_definitions(-D_WIN32_WINNT=0X0601)
            target_compile_definitions(${PROJECT_NAME} PUBLIC -D_WIN32_WINNT=0X0601)
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
	target_include_directories(${PROJECT_NAME} PUBLIC client license server ${Boost_INCLUDE_DIRS})
	add_subdirectory ("client")
	add_subdirectory ("license")
	add_subdirectory("server")

        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/itvpn.ini DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
	if(CMAKE_HOST_WIN32)
		#vc142-mt-x64-1_71.dll
		set(DEPENDENCIES ${BOOST_ROOT}bin/boost_system-vc142-mt-x64-1_71.dll
				${BOOST_ROOT}bin/boost_filesystem-vc142-mt-x64-1_71.dll
				${BOOST_ROOT}bin/boost_program_options-vc142-mt-x64-1_71.dll)
		foreach(DEPENDENCY ${DEPENDENCIES})
                    file(COPY ${DEPENDENCY} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
                    #???install(PROGRAMS "${DEPENDENCY}" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
		endforeach()
	elseif(CMAKE_HOST_UNIX)
		message("CMAKE_HOST_UNIX")
	endif()

endif()
