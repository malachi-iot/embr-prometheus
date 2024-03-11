cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

list(APPEND EXTRA_COMPONENT_DIRS ${CMAKE_CURRENT_LIST_DIR}/../../../src/esp-idf)

# DEBT: Want to enable this - however, if we do, we have to manually scoop up estd
# again which feels clumsy - I'd like to inherit that dependency from embr-prometheus
#set(COMPONENTS main)

project(basic-prometheus)
