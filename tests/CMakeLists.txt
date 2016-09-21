cmake_minimum_required(VERSION 3.6)

add_executable(libbeatnik_bench
    benchmark.cpp
)
target_link_libraries(libbeatnik_bench ${PROJECT_NAME})
target_link_libraries(libbeatnik_bench benchmark)
