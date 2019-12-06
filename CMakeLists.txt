# CMakeList.txt: файл проекта CMake верхнего уровня; проведите глобальную конфигурацию
# и включите сюда подпроекты.
#
cmake_minimum_required (VERSION 3.8)

project ("agent_license_checker")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)

find_package(Boost COMPONENTS chrono filesystem REQUIRED) 

# Включите подпроекты.
add_subdirectory ("client_license")
add_subdirectory ("license_checker")

add_executable(${PROJECT_NAME} application.cpp)

target_link_libraries(${PROJECT_NAME} client_license)
