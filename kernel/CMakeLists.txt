file(GLOB_RECURSE SOURCES "*.c" "*.S")
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} spike_interface)
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/kernel.lds")