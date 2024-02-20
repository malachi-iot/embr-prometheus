# For more information about build system see
cmake_minimum_required(VERSION 3.16)

# DEBT: All these relative paths seem to work even when symlinked... but I am concerned

include(${CMAKE_CURRENT_LIST_DIR}/../../../setvars.cmake)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# DEBT: This one line is not inline with recommended procedure.  pytest procedure
# somehow has a different mechanism to notice where the component lives
list(APPEND EXTRA_COMPONENT_DIRS ${CMAKE_CURRENT_LIST_DIR}/../..)

set(COMPONENTS main)

project(embr-prometheus-unity)
