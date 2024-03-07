# DEBT: Due to PGESP-40 we direct-inject core cpp files here
include(${CMAKE_CURRENT_LIST_DIR}/../setvars.cmake)
set(CORE ${ROOT_DIR}/src/core)
include(${CORE}/sources.cmake)
list(TRANSFORM SOURCES PREPEND ${CORE}/)

# DEBT: Although we really like esp component manager, there are circumstances
# we want to pull estd from somewhere else.  Add an override mechanism, if we can.
# Otherwise we're stuck using a different project/component entirely

idf_component_register(SRCS "embr-prometheus.c" ${SOURCES}
    INCLUDE_DIRS "include" ${CORE}/include
    REQUIRES estd # embr
    )
