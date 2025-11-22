cmake_minimum_required(VERSION 3.20)
project(coursework)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES
        "src/*.cpp"
)

add_executable(coursework ${SOURCES}
        src/concurrent/ConcurrentMap.h
        src/concurrent/ConcurrentMap.cpp
        src/threading/ThreadPool.cpp
        src/threading/ThreadPool.h)
