cmake_minimum_required(VERSION 3.4)
project(beatnik VERSION 1.0.0 LANGUAGES C CXX)

add_subdirectory(external/GSL)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall -Wextra -pedantic")

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include)
target_link_libraries(${PROJECT_NAME} INTERFACE gsl)

add_library(dummy include Beatnik.cpp)
target_link_libraries(dummy ${PROJECT_NAME})

add_subdirectory(tests)

