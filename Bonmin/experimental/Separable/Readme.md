# CMake-build

## Assumptions
- Bonmin was build including it's dependencies (with AMPL)
- pkg-config is valid

## Steps

### Prep CMake-script
Modify:

     set(COIN_DIR "X:/msys64_coin_v3/usr/local/coin")

Will be used to read pkg-config folder!

### CMake

    mkdir build
    cd build
    cmake -G "Unix Makefiles" ..

### Make

    make

# Warnings
Some small hacks to make it compile on my MinGW64-based builds (`gcc version 9.2.0 (Rev2, Built by MSYS2 project)`):

- `#endif` macro was hacked away without real analysis
    - see commit `f92ce7d55e8091cdf7187327328590f9c6157ead`
    - broke build
- `-fpermissive` was added without real analysis
    - see commit `CMakeLists.txt`
    - broke build
