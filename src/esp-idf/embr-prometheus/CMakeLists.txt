# DEBT: Due to PGESP-40 we direct-inject core cpp files here
include(${CMAKE_CURRENT_LIST_DIR}/../setvars.cmake)
set(CORE ${ROOT_DIR}/src/core)
include(${CORE}/sources.cmake)
list(TRANSFORM SOURCES PREPEND ${CORE}/)

idf_component_register(SRCS "embr-prometheus.c" ${SOURCES}
    INCLUDE_DIRS "include" ${CORE}/include
    REQUIRES estd # embr
    )
