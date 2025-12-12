project(drwlm-lib)

add_library(drwlm SHARED)
target_include_directories(drwlm PUBLIC include)
target_sources(drwlm PRIVATE
    src/drwlm.c)
