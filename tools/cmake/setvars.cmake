get_filename_component(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../.. ABSOLUTE)
set(TOOLS_DIR ${ROOT_DIR}/tools)

include(FetchContent)

set(FETCHCONTENT_QUIET FALSE)

get_filename_component(UNITY_SOURCES_DIR ${ROOT_DIR}/test/unity ABSOLUTE)

