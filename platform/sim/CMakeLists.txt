set(CMAKE_CXX_FLAGS "-g -Wall  -std=c++11 -pthread")

add_library(platform SHARED platform.c timer.c net/simplenic.c init.c)

target_link_libraries(platform netlib)

INSTALL(TARGETS platform DESTINATION "${PROJECT_SOURCE_DIR}/bin")
