cmake_minimum_required (VERSION 3.8)

#GroupSources(include/boost/beast beast)
#GroupSources(example/http/server/async "/")

#file (GLOB_RECURSE BOOST_BEAST_FILES    ${PROJECT_SOURCE_DIR}/include/boost/beast/*.hpp    ${PROJECT_SOURCE_DIR}/include/boost/beast/*.ipp)

add_executable (http-server-async http_server_async.cpp
)

#target_link_libraries(http-server-async    lib-asio    lib-asio-ssl    lib-beast)

set_property(TARGET http-server-async PROPERTY FOLDER "example-http-server")
target_include_directories(http-server-async PUBLIC ${Boost_INCLUDE_DIRS})