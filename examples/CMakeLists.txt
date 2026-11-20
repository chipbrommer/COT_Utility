cmake_minimum_required(VERSION 3.16)
project(cot_example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(cot_examples main.cpp)
target_link_libraries(cot_examples PRIVATE cot_utility)