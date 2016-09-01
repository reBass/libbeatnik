cmake_minimum_required(VERSION 3.4)
project(beatnik VERSION 1.0.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(EXTERNAL_SRC_DIR ${PROJECT_SOURCE_DIR}/external)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall -Wextra -pedantic")
set(SRC_DIR ${PROJECT_SOURCE_DIR}/include)

set(GSL_include_directories
    external/GSL/include
)

include_directories(BEFORE ${GSL_include_directories} ${SRC_DIR})
set(SRC
        ${GSL_include_directories}

        ${SRC_DIR}/beatnik/common/Element.hpp
        ${SRC_DIR}/beatnik/common/Index_iterator.hpp
        ${SRC_DIR}/beatnik/common/math.hpp
        ${SRC_DIR}/beatnik/common/Ring_array.hpp
        ${SRC_DIR}/beatnik/common/Pair_iterator.hpp

        ${SRC_DIR}/beatnik/fft/ACF.hpp
        ${SRC_DIR}/beatnik/fft/FFT.hpp
        ${SRC_DIR}/beatnik/fft/Hann_window.hpp
        ${SRC_DIR}/beatnik/fft/Real_FFT.hpp

        ${SRC_DIR}/beatnik/odf/Onset_detector.hpp

        ${SRC_DIR}/beatnik/decoder/Decoder.hpp
        ${SRC_DIR}/beatnik/decoder/Viterbi.hpp

        ${SRC_DIR}/beatnik/tracker/Skewed_window.hpp
        ${SRC_DIR}/beatnik/tracker/Tracker.hpp

        ${SRC_DIR}/beatnik/Beatnik.hpp

        ${PROJECT_SOURCE_DIR}/Beatnik.cpp
)
add_library(${PROJECT_NAME} STATIC ${SRC})

find_package(Benchmark REQUIRED)
add_executable(libbeatnik_bench
        ${PROJECT_SOURCE_DIR}/tests/benchmark.cpp
        ${PROJECT_SOURCE_DIR}/tests/kiss_fft/kiss_fft.c
        ${PROJECT_SOURCE_DIR}/tests/kiss_fft/kiss_fftr.c
)
target_link_libraries(libbeatnik_bench ${PROJECT_NAME})
target_link_libraries(libbeatnik_bench ${benchmark_LIBRARIES})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

