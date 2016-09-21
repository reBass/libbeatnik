cmake_minimum_required(VERSION 3.6)
project(beatnik VERSION 1.0.0 LANGUAGES C CXX)

add_subdirectory(external/reLib)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "cmake")

add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} INTERFACE include)
target_link_libraries(${PROJECT_NAME} INTERFACE reLib)

add_library(dummy include beatnik.cpp)
target_link_libraries(dummy ${PROJECT_NAME})

add_subdirectory(tests)

