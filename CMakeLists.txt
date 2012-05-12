cmake_minimum_required (VERSION 2.6)
project (Hive)

set(hive_version_major 0)
set(hive_version_minor 1)

configure_file (
	"${PROJECT_SOURCE_DIR}/include/config.h.in"
	"${PROJECT_BINARY_DIR}/include/config.h"
	)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/include")

add_library(hive core/scheduler.c net/packet.c)
add_subdirectory(platform)
