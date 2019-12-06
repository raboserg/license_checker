cmake_minimum_required (VERSION 3.8)

project ("license_checker")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS chrono filesystem REQUIRED) 

add_executable(${PROJECT_NAME} application.cpp application.h)

target_link_libraries(${PROJECT_NAME} license)
target_include_directories(${PROJECT_NAME} PUBLIC client license)

add_subdirectory ("client")
add_subdirectory ("license")
