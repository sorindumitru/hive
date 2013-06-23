set(CMAKE_CXX_FLAGS "-WALL -std=c++11 -pthread")

add_library(netlib net/netlib.c net/routing/routing.c net/nic.c node.c)

SET_TARGET_PROPERTIES(netlib PROPERTIES COMPILE_FLAGS -fPIC)
INSTALL(TARGETS netlib DESTINATION "${PROJECT_SOURCE_DIR}/bin")
