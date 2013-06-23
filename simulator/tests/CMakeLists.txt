add_library(simple SHARED simple.c)
add_library(timers SHARED timers.c)

INSTALL(TARGETS simple timers DESTINATION "${PROJECT_SOURCE_DIR}/bin/tests")
