cmake_minimum_required (VERSION 3.8)

#GroupSources(include/boost/beast beast)
#GroupSources(example/http/server/async "/")

#file (GLOB_RECURSE BOOST_BEAST_FILES    ${PROJECT_SOURCE_DIR}/include/boost/beast/*.hpp    ${PROJECT_SOURCE_DIR}/include/boost/beast/*.ipp)
add_executable(http-server-async
    http_server_async.cpp
    listener.cpp listener.h session.cpp session.h
    mime_types.cpp mime_types.h handle_request.h)

if(CMAKE_HOST_WIN32)
        target_link_libraries(http-server-async template)
elseif(CMAKE_HOST_UNIX)
        target_link_libraries(http-server-async template pthread)
endif()

#set_property(TARGET http-server-async PROPERTY FOLDER "example-http-server")
target_include_directories(http-server-async PUBLIC ../template ${Boost_INCLUDE_DIRS})
