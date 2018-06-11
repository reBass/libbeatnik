## Libbeatnik
C++17 library implementing a beat detection algorithm.

[See this excellent tutorial about the Beat Detection Algorithm](http://archive.gamedev.net/archive/reference/programming/features/beatdetection/)

## Compilation
Requires `cmake 3.11+`

Example using make

    git clone --recursive https://github.com/reBass/libbeatnik.git
    cd libbeatnik
    mkdir -p build
    cd build
    cmake ..
    make

## Running benchmarks

Build the library e.g. in the `build` directory and run the benchmark.

    ./build/tests/libbeatnik_bench

<img src="/docs/curly_libbeatnik_bench.png" alt="Example of benchamerk of libbeatnik result"/>
