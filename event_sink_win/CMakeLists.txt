cmake_minimum_required (VERSION 3.8)

set(SOURCES_LIST 
		eventsink.cpp
		eventsink.h
		notificator_win.cpp
		notificator_win.h
		main.cpp)

add_definitions(-D_UNICODE -DUNICODE)
add_library(eventsink STATIC ${SOURCES_LIST})
target_link_libraries(eventsink client)
#target_include_directories(license PUBLIC ${Boost_INCLUDE_DIRS})
