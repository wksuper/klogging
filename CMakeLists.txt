project(klogging)
cmake_minimum_required(VERSION 3.16)

include_directories(include)

add_library(klogging
        SHARED
        src/klogging.cpp
)

add_executable(myprog
        example/myprog.c
)
target_link_libraries(myprog
        klogging
)
