find_package(Benchmark REQUIRED)
add_subdirectory(external/kiss_fft)
add_subdirectory(external/catch)

add_executable(libbeatnik_bench
    benchmark.cpp
)
target_link_libraries(libbeatnik_bench ${PROJECT_NAME})
target_link_libraries(libbeatnik_bench catch)
target_link_libraries(libbeatnik_bench kiss_fft)
target_link_libraries(libbeatnik_bench ${benchmark_LIBRARIES})
