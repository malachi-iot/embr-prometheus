cmake_minimum_required(VERSION 3.27)
project(embr-prometheus)

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME} main.cpp)

add_library(embr::prometheus ALIAS ${PROJECT_NAME})
