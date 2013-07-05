set(CMAKE_CXX_FLAGS "-WALL -std=c++11 -pthread")

add_library(netlib net/netlib.c net/routing/routing.c net/nic.c node.c net/routing/broadcast.c net/routing/norouter.c net/routing/leach.c)

SET_TARGET_PROPERTIES(netlib PROPERTIES COMPILE_FLAGS "-fPIC -g")
INSTALL(TARGETS netlib DESTINATION "${PROJECT_SOURCE_DIR}/bin")
