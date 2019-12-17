cmake_minimum_required (VERSION 3.8)

set(SOURCE_LIB NLTemplate.cpp NLTemplate.h)

#if(CMAKE_HOST_WIN32)
#    set(JINJA2CPPLIGHT_DIR D:/library/vcpkg/packages/jinja2cpplight_x64-windows)
#    set(JINJA2CPPLIGHT_LIB_NAME "Jinja2CppLight")
#    set(JINJA2CPPLIGHT_DIR_ROOT ${JINJA2CPPLIGHT_DIR})
#    set(JINJA2CPPLIGHT_HEADER_DIR ${JINJA2CPPLIGHT_DIR_ROOT}/include)
#    LINK_DIRECTORIES("${JINJA2CPPLIGHT_DIR_ROOT}/debug/lib")
#elseif(CMAKE_HOST_UNIX)
#    set(CPPREST_DIR ~/projects/vcpkg/packages/cpprestsdk_x64-linux)
#    set(CPPREST_LIB_NAME cpprest)
#    set(CPPREST_ROOT ${CPPREST_DIR})
#    set(CPPREST_HEADER_DIR ${CPPREST_ROOT}/include)
#    LINK_DIRECTORIES("${CPPREST_ROOT}/build.debug/Release/libs")
#endif()

#add_library(template  STATIC ${SOURCE_LIB})
#add_executable(template test.cpp ${SOURCE_LIB}) 
#target_link_libraries(template parser ${JINJA2CPPLIGHT_LIB_NAME})
#target_include_directories(template PUBLIC ../parser ${JINJA2CPPLIGHT_HEADER_DIR})


add_library(template  STATIC ${SOURCE_LIB})
#add_executable(template test.cpp ${SOURCE_LIB}) 
target_link_libraries(template parser)
target_include_directories(template PUBLIC ../parser)
