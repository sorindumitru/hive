add_library(simple SHARED simple.c)
add_library(timers SHARED timers.c)
add_library(network SHARED network.c)

target_link_libraries(network netlib)

INSTALL(TARGETS simple timers network DESTINATION "${PROJECT_SOURCE_DIR}/bin/tests")
