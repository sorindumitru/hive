set(CMAKE_CXX_FLAGS "-Wall  -std=c++11 -stdlib=libc++ -pthread")


add_library(platform SHARED platform.c)

INSTALL(TARGETS platform DESTINATION "${PROJECT_SOURCE_DIR}/bin")

include_directories("${PROJECT_SOURCE_DIR}/platform/sim/include")
add_executable(hive simulator/main.cc simulator/worker.cc)
target_link_libraries(hive platform ev)
INSTALL(TARGETS hive DESTINATION "${PROJECT_SOURCE_DIR}/bin")


