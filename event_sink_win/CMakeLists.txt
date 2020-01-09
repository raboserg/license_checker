cmake_minimum_required (VERSION 3.8)

set(SOURCES_LIST 
		eventsink.cpp
		eventsink.h
		notificator_win.cpp
		notificator_win.h
		main.cpp)


add_library(eventsink STATIC ${SOURCES_LIST})
target_link_libraries(eventsink client)
