cmake_minimum_required (VERSION 3.8)

set(SOURCE_LIB client_license.cpp client_license.h)

#set(${CPPREST_DIR} "/home/user/projects/library/casablanca/build.debug/Release/Binaries")
set(CPPREST_DIR "/home/user/projects/library/casablanca")
#/build.debug/Release/Binaries

#set(CPPREST_DIR ${VCPKG_DIR})

#/home/user/projects/library/casablanca/Release/include


if(CMAKE_HOST_WIN32)
    set(CPPREST_LIB_NAME "cpprest_2_10d")
elseif(CMAKE_HOST_UNIX)
    set(CPPREST_LIB_NAME cpprest)
endif()

#add_executable(client ${SOURCE_LIB})
add_library (client STATIC ${SOURCE_LIB})

find_library(CPPREST_LIB NAMES ${CPPREST_LIB_NAME} PATHS "${CPPREST_DIR}/build.debug/Release/Binaries")


if(CMAKE_HOST_WIN32)
    target_link_libraries(client ${CPPREST_LIB}) 
    target_include_directories(client PUBLIC ${CMAKE_INCLUDE_PATH} ${Boost_INCLUDE_DIRS})
elseif(CMAKE_HOST_UNIX)
    set(CMAKE_LIBRARY_PATH_ ${CMAKE_LIBRARY_PATH})
    set(CMAKE_INCLUDE_PATH_ ${CMAKE_INCLUDE_PATH})
    set(CMAKE_LIBRARY_PATH ${DEFAULT_UNIX_BOOST_DIR}/stage/lib)
    set(CMAKE_INCLUDE_PATH ${DEFAULT_UNIX_BOOST_DIR})
#    set(BOOST_INCLUDEDIR ${DEFAULT_UNIX_BOOST_DIR})
#    set(BOOST_LIBRARYDIR ${DEFAULT_UNIX_BOOST_DIR}/stage/lib)
    find_package(Boost COMPONENTS system)

    set(OPENSSL_LIBRARY_PATH "/home/user/projects/library/openssl")
    set(OPENSSL_INCLUDE_PATH "/home/user/projects/library/openssl/include")

    find_library(SSL_LIB ssl ${OPENSSL_LIBRARY_PATH})
    find_library(CRYPTO_LIB crypto ${OPENSSL_LIBRARY_PATH})

    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH_})
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH_})

    message(STATUS "CPPREST_DIR}/Release/includeCPPREST_DIR}/Release/includeCPPREST_DIR}/Release/include - ${CPPREST_LIB}")
    target_include_directories(client PUBLIC "${CPPREST_DIR}/Release/include" ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_PATH})
    target_link_libraries(client PUBLIC ${CPPREST_LIB} pthread ${Boost_LIBRARIES} ${SSL_LIB} ${CRYPTO_LIB} pthread dl)

endif()
