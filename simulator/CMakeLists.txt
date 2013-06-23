set(CMAKE_CXX_FLAGS "-g -Wall  -std=c++11 -pthread")

include_directories("${PROJECT_SOURCE_DIR}/simulator/include")

add_executable(hive main.cc worker.cc control.cc)

target_link_libraries(hive platform event dl jsoncpp)

INSTALL(TARGETS hive DESTINATION "${PROJECT_SOURCE_DIR}/bin")

add_subdirectory(tests)
