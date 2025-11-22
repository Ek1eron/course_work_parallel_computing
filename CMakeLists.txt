cmake_minimum_required(VERSION 3.20)
project(coursework)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES
        "src/*.cpp"
)

add_executable(coursework
        src/main.cpp
        src/index/InvertedIndex.cpp
        src/concurrent/ConcurrentMap.cpp
        src/server/SearchServer.cpp
        src/threading/ThreadPool.cpp
        src/utils/FileLoader.cpp
        src/utils/Tokenizer.cpp
)

target_link_libraries(coursework ws2_32)

add_executable(client src/client/Client.cpp)
target_link_libraries(client ws2_32)



if (WIN32)
    target_link_libraries(coursework ws2_32)
endif()