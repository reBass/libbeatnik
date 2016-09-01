cmake_minimum_required(VERSION 3.4)
project(kiss_fft LANGUAGES C)

add_library(${PROJECT_NAME}
        include
        src/_kiss_fft_guts.h
        src/kiss_fft.c
        src/kiss_fftr.c
)
target_include_directories(${PROJECT_NAME} PUBLIC include)
