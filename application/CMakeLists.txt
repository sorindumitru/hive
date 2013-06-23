set(CMAKE_CXX_FLAGS "-WALL -std=c++11 -pthread")

add_library(spin SHARED spin/spin.c)
target_link_libraries(spin netlib)
SET_TARGET_PROPERTIES(spin PROPERTIES COMPILE_FLAGS "-g")

INSTALL(TARGETS spin DESTINATION "${PROJECT_SOURCE_DIR}/bin")
