include(${CMAKE_CURRENT_LIST_DIR}/../../../../setvars.cmake)
include(${UNITY_SOURCES_DIR}/sources.cmake)
list(TRANSFORM UNITY_SOURCES PREPEND ${UNITY_SOURCES_DIR}/)

idf_component_register(SRCS "main.cpp" ${UNITY_SOURCES}
    INCLUDE_DIRS "."
    PRIV_REQUIRES unity embr-prometheus
#    WHOLE_ARCHIVE
    )
