add_subdirectory(deps)

file(GLOB SOURCES "*.c")
include_directories(deps)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --entry main")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/hostfs_root/bin)
foreach (SOURCE ${SOURCES})
    get_filename_component(NAME ${SOURCE} NAME_WE)
    add_executable(${NAME} ${SOURCE})
    target_link_libraries(${NAME} PRIVATE user)
endforeach ()