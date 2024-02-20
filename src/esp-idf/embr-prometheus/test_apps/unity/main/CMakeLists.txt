include(${CMAKE_CURRENT_LIST_DIR}/../../../../setvars.cmake)
include(${UNITY_SOURCES_DIR}/sources.cmake)
list(TRANSFORM UNITY_SOURCES PREPEND ${UNITY_SOURCES_DIR}/)

idf_component_register(SRCS "main.cpp" ${UNITY_SOURCES}
    INCLUDE_DIRS "."
    # Fails to resolve, despite FetchContent'ing in
    #REQUIRES embr estd
    PRIV_REQUIRES unity embr-prometheus
    # NOTE: This line is needed for Unity to auto-scoop the tests
    WHOLE_ARCHIVE
    )

# DEBT: See PGESP-40
#add_subdirectory(${ROOT_DIR}/src/core core)

#target_link_libraries(${COMPONENT_LIB} PUBLIC embr::prometheus::core)