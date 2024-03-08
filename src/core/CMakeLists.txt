cmake_minimum_required(VERSION 3.19)
project(embr-prometheus-core VERSION 0.1.0
    DESCRIPTION "Embedded Prometheus provider"
    HOMEPAGE_URL "https://github.com/malachi-iot/embr-prometheus")

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

# DEBT: Making this INTERFACE as we work out linker issues (it didn't help, though)
# Put it back to default linkage once xsputn/sputc is worked out

set(LINKAGE_MODE PUBLIC)

list(TRANSFORM INCLUDES PREPEND include/embr/)
list(TRANSFORM EXTRAS PREPEND ${ROOT_DIR}/)

#add_library(${PROJECT_NAME} ${LINKAGE_MODE} ${SOURCES})
add_library(${PROJECT_NAME})

target_sources(${PROJECT_NAME} ${LINKAGE_MODE}
    ${SOURCES}
    ${INCLUDES}
    ${EXTRAS}
)

#add_library(embr::prometheus ALIAS ${PROJECT_NAME})
add_library(embr::prometheus::core ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} ${LINKAGE_MODE} include)

include(${TOOLS_DIR}/cmake/version.cmake)

target_link_libraries(${PROJECT_NAME} ${LINKAGE_MODE} embr estd)

# Makes no difference for estd "that is not in any export set." error
#target_link_libraries(${PROJECT_NAME} PUBLIC idf::newlib)
