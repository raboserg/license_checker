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
    target_link_libraries(client ${CPPREST_LIB}) 
    target_include_directories(client PUBLIC ../tracer ../tools ${Boost_INCLUDE_DIRS} ${DEFAULT_LIBP7_INCLUDE_DIR})
elseif(CMAKE_HOST_UNIX)
    set(CPPREST_LIB_NAME cpprest)
    find_library(CPPREST_LIB NAMES ${CPPREST_LIB_NAME} PATHS ${CPPREST_LIBRARY_DIR})

#    set(CMAKE_LIBRARY_PATH_ ${CMAKE_LIBRARY_PATH})
#    set(CMAKE_INCLUDE_PATH_ ${CMAKE_INCLUDE_PATH})
#    set(CMAKE_LIBRARY_PATH ${DEFAULT_UNIX_BOOST_DIR}/stage/lib)
#    set(CMAKE_INCLUDE_PATH ${DEFAULT_UNIX_BOOST_DIR})
##    set(BOOST_INCLUDEDIR ${DEFAULT_UNIX_BOOST_DIR})
##    set(BOOST_LIBRARYDIR ${DEFAULT_UNIX_BOOST_DIR}/stage/lib)
#    find_package(Boost COMPONENTS system)
#    #set(OPENSSL_LIBRARY_PATH "/home/user/projects/library/openssl")
#    #set(OPENSSL_INCLUDE_PATH "/home/user/projects/library/openssl/include")
#    find_library(SSL_LIB ssl ${OPENSSL_LIBRARY_PATH})
#    find_library(CRYPTO_LIB crypto ${OPENSSL_LIBRARY_PATH})
#    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH_})
#    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH_})
   
    target_include_directories(client PRIVATE ../tracer ../tools 
		${CPPREST_INCLUDE_DIR} 
		${Boost_INCLUDE_DIRS} 
		${OPENSSL_INCLUDE_PATH} 
		${DEFAULT_LIBP7_INCLUDE_DIR})
    target_link_libraries(client PRIVATE ${CPPREST_LIB} ${Boost_LIBRARIES} ${SSL_LIB} ${CRYPTO_LIB} pthread dl)

endif()
