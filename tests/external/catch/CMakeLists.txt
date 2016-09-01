project(Catch VERSION 1.5.6 LANGUAGES CXX)

add_library(catch INTERFACE)
target_include_directories(catch INTERFACE include)
