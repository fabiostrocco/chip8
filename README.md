# Build

## Windows

Open builds\vs\chip8.sln and build

## Linux

Run (the first step may be skipped)

> sudo apg-get update
> sudo apt install cmake libsdl2-dev g++
> cd builds/cmake
> cmake .
> make all
> ./chip8

# Testing

## Windows

Open builds\vs\chip8.sln and run tests (ensure GoogleTest is installed in Visual Studio)

## Linux

> cmake .
> make all
> ./chip8-test

# Assembler

Use nasm hello_world.nasm to create a chip8 file (taken from https://github.com/mfurga/chip8)

ROMS: https://github.com/kripod/chip8-roms