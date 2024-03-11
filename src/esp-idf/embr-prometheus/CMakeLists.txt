# DEBT: Due to PGESP-40 we direct-inject core cpp files here
include(${CMAKE_CURRENT_LIST_DIR}/../setvars.cmake)
set(CORE ${ROOT_DIR}/src/core)
include(${CORE}/sources.cmake)
list(TRANSFORM SOURCES PREPEND ${CORE}/)

# As per https://github.com/espressif/esp-idf/issues/9929
if(NOT CMAKE_BUILD_EARLY_EXPANSION)
    include(${TOOLS_DIR}/cmake/fetch.cmake)
endif()


# DEBT: Although we really like esp component manager, there are circumstances
# we want to pull estd from somewhere else.  Add an override mechanism, if we can.
# Otherwise we're stuck using a different project/component entirely

idf_component_register(SRCS "embr-prometheus.c" ${SOURCES}
    INCLUDE_DIRS "include" ${CORE}/include ${quantiles_SOURCE_DIR}
    REQUIRES estd # embr
    )
