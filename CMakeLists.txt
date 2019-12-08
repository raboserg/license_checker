cmake_minimum_required (VERSION 3.8)

project ("license_checker")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
#find_package(Boost COMPONENTS system filesystem process REQUIRED)
#find_package(Boost system filesystem process REQUIRED)
find_package(Boost REQUIRED)
add_executable(${PROJECT_NAME} application.cpp application.h)

target_link_libraries(${PROJECT_NAME} license pthread
    boost_system boost_filesystem boost_program_options)
target_include_directories(${PROJECT_NAME} PUBLIC client license)

add_subdirectory ("client")
add_subdirectory ("license")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/itvpn.ini
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
