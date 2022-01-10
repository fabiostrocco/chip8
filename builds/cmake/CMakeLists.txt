cmake_minimum_required(VERSION 3.10)

# ========================================= chip8 virtual machine =========================================
project(chip8)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(chip8 ../../src/main.cpp)
target_link_libraries(chip8 ${SDL2_LIBRARIES})

# ========================================= chip8 testing =================================================

add_subdirectory(gtest)
add_executable(chip8-test ../../test/Test.cpp main.cpp)
add_test(NAME chip8-test COMMAND mytest)
target_link_libraries(chip8-test gtest)