# CMakeList.txt: проект CMake для license_checker; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

project(client_license)

set(SOURCE_LIB client_license.cpp client_license.h)

add_library (client_license STATIC ${SOURCE_LIB})
