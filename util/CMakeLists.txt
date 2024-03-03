
file(GLOB_RECURSE SOURCES "*.c" "*.S")


add_library(
        util
        ${SOURCES}
)