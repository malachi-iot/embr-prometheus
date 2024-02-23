cmake_minimum_required(VERSION 3.19)

project(embr-prometheus-posix-test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(../../src/core core)

add_executable(${PROJECT_NAME} main.cpp)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

target_link_libraries(${PROJECT_NAME} embr::prometheus::core)

if(WIN32)
  target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
endif()
