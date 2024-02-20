cmake_minimum_required(VERSION 3.27)
project(embr-prometheus-test)

set(CMAKE_CXX_STANDARD 17)

include(../../tools/cmake/setvars.cmake)

add_subdirectory(${ROOT_DIR}/src/core core)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY  https://github.com/catchorg/Catch2.git
    GIT_TAG         v3.5.2
    GIT_SHALLOW     TRUE
)

FetchContent_MakeAvailable(Catch2)


add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
    embr::prometheus
    Catch2::Catch2WithMain)
