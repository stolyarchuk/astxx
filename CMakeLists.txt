project(astxx)
cmake_minimum_required(VERSION 2.8)
include_directories(./include)
aux_source_directory(./src SRC_DIR)
add_library(${PROJECT_NAME} SHARED ${SRC_DIR})
target_link_libraries(${PROJECT_NAME} boost_system)

#add_custom_target(PROJECT_NAME SOURCES "./include/")

add_executable(manager examples/manager.cpp)
target_link_libraries(manager astxx)

add_executable(queue-event-test examples/queue-event-test.cpp)
target_link_libraries(queue-event-test astxx)
