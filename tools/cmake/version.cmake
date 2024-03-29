include(${CMAKE_CURRENT_LIST_DIR}/setvars.cmake)

set(PROJECT_NAME_UPPER EMBR_PROMETHEUS)

# NOTE: Try to consolidate these scripts/inputs and estd

message(STATUS "estd_SOURCE_DIR=${estd_SOURCE_DIR}")

configure_file(
    ${estd_SOURCE_DIR}/tools/cmake/in/version.in.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/embr/prometheus/version.h)

configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/in/idf_component.in.yml
    ${ROOT_DIR}/idf_component.yml)
