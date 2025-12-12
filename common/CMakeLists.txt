project(drwlm-common)

add_library(drwlm-common INTERFACE)
target_include_directories(drwlm-common INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
