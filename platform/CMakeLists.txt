set(CMAKE_CXX_FLAGS "-g -Wall  -std=c++11 -pthread")


add_library(platform SHARED platform.c)

INSTALL(TARGETS platform DESTINATION "${PROJECT_SOURCE_DIR}/bin")
