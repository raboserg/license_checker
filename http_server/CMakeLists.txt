cmake_minimum_required (VERSION 3.8)

#GroupSources(include/boost/beast beast)
#GroupSources(example/http/server/async "/")

#file (GLOB_RECURSE BOOST_BEAST_FILES    ${PROJECT_SOURCE_DIR}/include/boost/beast/*.hpp    ${PROJECT_SOURCE_DIR}/include/boost/beast/*.ipp)

message("CMAKE_PREFIX_PATH - ${CMAKE_PREFIX_PATH}")

add_executable(http-server-async
    http_server_async.cpp
    listener.cpp listener.h session.cpp session.h
    request_handler.cpp request_handler.h
    mime_types.cpp mime_types.h)

#target_link_libraries(http-server-async lib-asio lib-asio-ssl lib-beast pthread)
target_link_libraries(http-server-async pthread)

set_property(TARGET http-server-async PROPERTY FOLDER "example-http-server")
target_include_directories(http-server-async PUBLIC ${Boost_INCLUDE_DIRS})
