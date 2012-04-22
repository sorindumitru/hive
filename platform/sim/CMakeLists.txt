set(CMAKE_CXX_FLAGS "-Wall  -std=c++0x -pthread")

include_directories("${PROJECT_SOURCE_DIR}/platform/sim/include")

add_executable(hivesim main.cpp lib/module.cpp core/scheduler.cc)
target_link_libraries(hivesim dl event hive)
