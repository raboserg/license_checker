cmake_minimum_required (VERSION 3.8)

set(SOURCE_LIB client_license.cpp client_license.h)

add_library (client STATIC ${SOURCE_LIB})

if(CMAKE_HOST_WIN32)
    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set(CPPREST_LIB_NAME "cpprest_2_10d")
    else()
        set(CPPREST_LIB_NAME "cpprest_2_10")
    endif()
    find_library(CPPREST_LIB NAMES ${CPPREST_LIB_NAME})
    target_link_libraries(client PRIVATE ${CPPREST_LIB} constants) 
    target_include_directories(client PUBLIC ../tracer ../tools ../constants ${Boost_INCLUDE_DIRS} ${DEFAULT_LIBP7_INCLUDE_DIR})
elseif(CMAKE_HOST_UNIX)
    set(CPPREST_LIB_NAME cpprest)
    find_library(CPPREST_LIB NAMES ${CPPREST_LIB_NAME} PATHS ${CPPREST_LIBRARY_DIR})
    target_include_directories(client PRIVATE ../tracer ../tools ../constants
        ${ACE_INCLUDE_DIR}
        ${CPPREST_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${OPENSSL_INCLUDE_PATH}
        ${DEFAULT_LIBP7_INCLUDE_DIR})
    target_link_libraries(client PRIVATE constants ${CPPREST_LIB} ${Boost_LIBRARIES} ${SSL_LIB} ${CRYPTO_LIB} pthread dl)

endif()
