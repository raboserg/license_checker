cmake_minimum_required (VERSION 3.8)

set(SOURCE_LIB NLTemplate.cpp NLTemplate.h)

if(CMAKE_HOST_WIN32)
    set(JINJA2CPP_DIR D:/library/vcpkg/packages/jinja2cpp_x64-windows)
elseif(CMAKE_HOST_UNIX)
    set(JINJA2CPP_DIR ~/projects/vcpkg/packages/jinja2cpp_x64-windows)
endif()

set(JINJA2CPP_LIB_NAME "jinja2cpp")
set(JINJA2CPP_DIR_ROOT ${JINJA2CPP_DIR})
set(JINJA2CPP_HEADER_DIR ${JINJA2CPP_DIR_ROOT}/include)
LINK_DIRECTORIES("${JINJA2CPP_DIR_ROOT}/debug/lib")

#add_library(template  STATIC ${SOURCE_LIB})
#add_executable(template test.cpp ${SOURCE_LIB})
add_executable(template test.cpp)
target_link_libraries(template parser ${JINJA2CPP_LIB_NAME})
target_include_directories(template PUBLIC ../parser ${JINJA2CPP_HEADER_DIR} ${JINJA2CPP_HEADER_DIR})

#target_link_libraries(template parser)
#target_include_directories(template PUBLIC ../parser)
