set(CMAKE_CXX_FLAGS "-WALL -std=c++11 -pthread")

add_library(netlib net/netlib.c net/routing/routing.c)
INSTALL(TARGETS netlib DESTINATION "${PROJECT_SOURCE_DIR}/bin")
