cmake_minimum_required(VERSION 3.2.2)
project(tcp-sync)

set(CMAKE_CXX_STANDARD 11)

include_directories(./)

set(CMAKE_CXX_FLAGS_DEBUG "-pipe -DDEBUG -Wall -ggdb -O0 ${CMAKE_CXX_FLAGS_ALL}")

set(CMAKE_CXX_FLAGS_RELEASE "-pipe -DNDEBUG -Wall -g0 -O2 -s ${CMAKE_CXX_FLAGS_ALL}")


file(GLOB SOURCE_EXCEPTION "Exception.cpp")

file(GLOB SOURCE_CLIENT "TcpClient.cpp")

#file(GLOB SOURCE_SERVER "TcpServer.cpp")

set(SOURCE_FILES main.cpp)

add_executable(tcp-test ${SOURCE_FILES} ${SOURCE_EXCEPTION} ${SOURCE_CLIENT})

target_link_libraries(tcp-test pthread)
