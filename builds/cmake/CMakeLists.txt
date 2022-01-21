cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ========================================= chip8 virtual machine =========================================
project(chip8)

find_package(SDL2 REQUIRED)

set(CHIP8_SRC "../../src/")
set(CHIP8_CLPARSER "${CHIP8_SRC}clparser/")
set(CHIP8_LOGGING "${CHIP8_SRC}logging/")
set(CHIP8_EMULATOR "${CHIP8_SRC}emulator/")

set(CHIP8_SOURCE_FILES
    "${CHIP8_SRC}main.cpp"
    "${CHIP8_SRC}Logger.cpp"
    
    "${CHIP8_CLPARSER}CommandLineOptions.cpp"
    "${CHIP8_CLPARSER}CommandLineParser.cpp"
	
    "${CHIP8_EMULATOR}AudioController.cpp"
    "${CHIP8_EMULATOR}Cpu.cpp"
    "${CHIP8_EMULATOR}Emulator.cpp"
    "${CHIP8_EMULATOR}EmulatorWindow.cpp"
    "${CHIP8_EMULATOR}Gpu.cpp"
    "${CHIP8_EMULATOR}Timer.cpp")
	
message("compiler=${CMAKE_CXX_COMPILER_ID} version=${CMAKE_CXX_COMPILER_VERSION}")

include_directories(${SDL2_INCLUDE_DIRS} ${CHIP8_SRC})

add_executable(chip8 ${CHIP8_SOURCE_FILES})
target_link_libraries(chip8 ${SDL2_LIBRARIES})

# ========================================= chip8 testing =================================================

add_subdirectory(gtest)

set(CHIP8_TEST "../../test/")
set(CHIP8_TEST_FILES
    "main.cpp"
    "${CHIP8_EMULATOR}Cpu.cpp"
    "${CHIP8_EMULATOR}Gpu.cpp"
    "${CHIP8_TEST}CommandLineParserUnitTests.cpp"
    "${CHIP8_TEST}CommandLineParserIntegrationTests.cpp"
    "${CHIP8_TEST}CpuUnitTests.cpp"
    "${CHIP8_TEST}GpuUnitTests.cpp"
    "${CHIP8_TEST}InstructionBinderUnitTests.cpp")

set(CHIP8_TEST_SOURCE_FILES
    "${CHIP8_CLPARSER}CommandLineOptions.cpp"
    "${CHIP8_CLPARSER}CommandLineParser.cpp"
    "${CHIP8_EMULATOR}Cpu.cpp")

set(CHIP8_SRC "../../src/")

include_directories(${CHIP8_SRC})
add_executable(chip8-test ${CHIP8_TEST_FILES} ${CHIP8_TEST_SOURCE_FILES})
add_test(NAME chip8-test COMMAND chip8-test)
target_link_libraries(chip8-test gtest gmock)
