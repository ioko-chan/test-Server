cmake_minimum_required(VERSION 3.23)
project(testServer)

set(CMAKE_CXX_STANDARD 14)

add_executable(testServer main.cpp)

if(MINGW)
    target_link_libraries(testServer ws2_32)
endif()