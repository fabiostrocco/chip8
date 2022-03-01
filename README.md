# Build

## Windows

Open builds\vs\chip8.sln and build

## Linux

Run (the first 2 steps may be skipped but they ensure you have gcc-11)

```console
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt update
sudo apt install cmake libsdl2-dev gcc-11
cd builds/cmake
cmake .
make all
./chip8
```

Make sure you are using a C++ compiler supporting concepts. If not, try the following

```console
cmake -D CMAKE_CXX_COMPILER=g++-11 
```

# Testing

## Windows

Open builds\vs\chip8.sln and run tests (ensure GoogleTest is installed in Visual Studio)

## Linux

```console
cmake .
make all
./chip8-test
```

# Assembler

Use nasm hello_world.nasm to create a chip8 file (taken from https://github.com/mfurga/chip8)

ROMS: https://johnearnest.github.io/chip8Archive/
