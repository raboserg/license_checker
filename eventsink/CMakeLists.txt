cmake_minimum_required (VERSION 3.8)

set(SOURCE_LIB 
	eventsink.cpp
	eventsink.h
	exec_notificator.cpp
	exec_notificator.h
	main.cpp
)
add_library(eventsink STATIC ${SOURCE_LIB})
add_definitions(-D_UNICODE -DUNICODE)
target_link_libraries(eventsink client)
#target_include_directories(license PUBLIC ${Boost_INCLUDE_DIRS})
