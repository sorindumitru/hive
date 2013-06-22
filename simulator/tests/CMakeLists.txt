add_library(simple simple.c)
add_library(timers timers.c)

INSTALL(TARGETS simple timers DESTINATION "${PROJECT_SOURCE_DIR}/bin/tests")
