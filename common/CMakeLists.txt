project(drwlm-common)

add_library(drwlm-common STATIC)

target_sources(drwlm-common PRIVATE
    src/ipc.c
    src/logging.c
    src/lock.c
)

target_include_directories(drwlm-common PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
