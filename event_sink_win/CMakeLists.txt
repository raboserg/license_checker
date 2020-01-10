cmake_minimum_required (VERSION 3.8)

set(SOURCES_LIST 
		eventsink.cpp
		eventsink.h
		notificator_win.cpp
		notificator_win.h
		main.cpp)

add_library(eventsink STATIC ${SOURCES_LIST})
target_compile_definitions(eventsink PRIVATE -D_WIN32_WINNT=0x0601)
target_link_libraries(eventsink client)
