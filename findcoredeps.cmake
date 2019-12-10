cmake_minimum_required(VERSION 3.5)

#set(CMAKE_CXX_STANDARD 14)
#cmake_policy(SET CMP0079 NEW)

set(BOOST_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

set(BOOST_INCLUDEDIR "")
set(BOOST_LIBRARYDIR "")

if (DEFINED ENV{DEP_DIR})
    message(WARNING "Overriding DEP_DIR setting with environment variable")
    set(DEP_DIR $ENV{DEP_DIR})
endif ()

# Include our DEP_DIR in path used to find libraries

#if (APPLE)
#    set(OPENVPN_PLAT osx)
#elseif (WIN32)
#    set(OPENVPN_PLAT amd64)
#else ()
#    set(OPENVPN_PLAT linux)
#endif ()

function(add_core_dependencies target)
    set(PLAT ${OPENVPN_PLAT})

    set(CORE_INCLUDES
            ${CORE_DIR}
            )
    set(CORE_DEFINES
            -DASIO_STANDALONE
            -DUSE_ASIO
            -DHAVE_LZ4
            -DLZ4_DISABLE_DEPRECATE_WARNINGS
            -DMBEDTLS_DEPRECATED_REMOVED
            )

    if (WIN32)
        list(APPEND CORE_DEFINES
                -D_WIN32_WINNT=0x0600
                -D_CRT_SECURE_NO_WARNINGS
                )
        #set(EXTRA_LIBS fwpuclnt.lib Iphlpapi.lib lz4::lz4 wininet.lib Wtsapi32.lib setupapi.lib rpcrt4.lib)
        #target_compile_options(${target} PRIVATE "/bigobj")

        find_package(lz4 CONFIG REQUIRED)
        list(APPEND CORE_INCLUDES ${ASIO_INCLUDE_DIR})
    else ()
        list(APPEND CORE_INCLUDES
                ${DEP_DIR}/asio/asio/include
                )
        list(APPEND CMAKE_PREFIX_PATH
                ${DEP_DIR}/mbedtls/mbedtls-${PLAT}
                ${DEP_DIR}/lz4/lz4-${PLAT}
                )
        list(APPEND CMAKE_LIBRARY_PATH
                ${DEP_DIR}/mbedtls/mbedtls-${PLAT}/library
                )

        find_package(LZ4 REQUIRED)
    endif ()

    if(UNIX)
        target_link_libraries(${target} pthread)
    endif()

    list(APPEND CORE_INCLUDES ${LZ4_INCLUDE_DIR})

    target_include_directories(${target} PRIVATE ${CORE_INCLUDES})
    target_compile_definitions(${target} PRIVATE ${CORE_DEFINES})
    target_link_libraries(${target} ${SSL_LIBRARY} ${EXTRA_LIBS} ${LZ4_LIBRARY})
endfunction()