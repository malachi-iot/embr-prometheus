cmake_minimum_required(VERSION 3.19)
project(embr-prometheus)

set(CMAKE_CXX_STANDARD 17)

include(../../tools/cmake/setvars.cmake)

FetchContent_Declare(
    embr
    SOURCE_SUBDIR   src
    GIT_REPOSITORY  git@github.com:malachi-iot/embr.git
    # DEBT: Attach this to a proper release tag
    GIT_TAG         alpha
    GIT_SHALLOW     TRUE
)

FetchContent_MakeAvailable(embr)

add_library(${PROJECT_NAME} main.cpp)

add_library(embr::prometheus ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME} PUBLIC embr)
