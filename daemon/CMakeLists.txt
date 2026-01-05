project(drwlm-daemon)

add_executable(drwlmd)
target_include_directories(drwlmd PRIVATE include)
target_sources(drwlmd PRIVATE
    src/drwlmd.c
    src/pidfile.c
    src/context.c
    src/corosync.c
    src/engine.c)

target_link_libraries(drwlmd drwlm-common cpg corosync_common)
