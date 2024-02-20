cmake_minimum_required(VERSION 3.19)
project(embr-prometheus-core)

set(CMAKE_CXX_STANDARD 17)

# Neither INTERFACE on or off works.  Two separate rabbit holes
#set(FEATURE_ESTD_INTERFACE 1 CACHE BOOL "Due to mysterious link errors")

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

include(sources.cmake)

add_library(${PROJECT_NAME} ${SOURCES})

#add_library(embr::prometheus ALIAS ${PROJECT_NAME})
add_library(embr::prometheus::core ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME} PUBLIC embr estd)

# Makes no difference for estd "that is not in any export set." error
#target_link_libraries(${PROJECT_NAME} PUBLIC idf::newlib)
