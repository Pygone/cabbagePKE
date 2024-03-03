

file(GLOB_RECURSE SPIKE_INTERFACE_SRC "*.c")
add_library(
        spike_interface
        ${SPIKE_INTERFACE_SRC}
)
target_link_libraries(spike_interface
        PUBLIC
        util)