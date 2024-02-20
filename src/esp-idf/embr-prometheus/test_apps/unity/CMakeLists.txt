# For more information about build system see
cmake_minimum_required(VERSION 3.16)

# DEBT: All these relative paths seem to work even when symlinked... but I am concerned

include(${CMAKE_CURRENT_LIST_DIR}/../../../setvars.cmake)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

FetchContent_Declare(
    embr
    #SOURCE_SUBDIR   src
    GIT_REPOSITORY  git@github.com:malachi-iot/embr.git
    # DEBT: Attach this to a proper release tag
    GIT_TAG         alpha
    GIT_SHALLOW     TRUE
)

FetchContent_MakeAvailable(embr)

message(DEBUG "EMBR: ${embr_SOURCE_DIR}")

# DEBT: This one line is not inline with recommended procedure.  pytest procedure
# somehow has a different mechanism to notice where the component lives
list(APPEND EXTRA_COMPONENT_DIRS ${CMAKE_CURRENT_LIST_DIR}/../..)

# NOTE: Ultra clunky, but at least it is reliable enough to not be DEBT
# (though we'd prefer PGESP-40 to resolve and clean out this whole mess)
list(APPEND EXTRA_COMPONENT_DIRS
    ${embr_SOURCE_DIR}/ext/estdlib/tools/esp-idf/components
    ${embr_SOURCE_DIR}/tools/esp-idf/components
    )

set(COMPONENTS main)

project(embr-prometheus-unity)
