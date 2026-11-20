cmake_minimum_required(VERSION 3.16)
project(cot_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_executable(test_cot test_cot.cpp)
target_link_libraries(test_cot PRIVATE cot_utility)
add_test(NAME cot_tests COMMAND test_cot)