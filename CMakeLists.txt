project(klogging)
cmake_minimum_required(VERSION 3.16)

include_directories(include)

add_library(klogging SHARED
        src/klogging.cpp
)
if(CONFIG_SUPPORT_LUA EQUAL 1)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -DCONFIG_SUPPORT_LUA)
target_link_libraries(klogging
        lua
)
endif()
set_target_properties(klogging PROPERTIES
        PUBLIC_HEADER include/klogging.h
)

add_executable(myprog
        example/myprog.c
)
target_link_libraries(myprog
        klogging
)

install(TARGETS
        klogging
)
