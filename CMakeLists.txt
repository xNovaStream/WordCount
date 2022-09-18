cmake_minimum_required(VERSION 3.23)
project(WordCount)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(WordCount main.cpp)
