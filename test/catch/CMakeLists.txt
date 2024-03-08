cmake_minimum_required(VERSION 3.19)
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


add_executable(${PROJECT_NAME} main.cpp ostream.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
    embr::prometheus::core
    Catch2::Catch2WithMain)

target_compile_definitions(${PROJECT_NAME} PRIVATE UNIT_TESTING=1)
