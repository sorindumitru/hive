add_library(simple SHARED simple.c)
add_library(timers SHARED timers.c)
add_library(complex SHARED complex.c)

INSTALL(TARGETS simple timers complex DESTINATION "${PROJECT_SOURCE_DIR}/bin/tests")
