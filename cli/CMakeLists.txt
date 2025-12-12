project(drwlm-cli)

add_executable(drwlmtool)
target_include_directories(drwlmtool PRIVATE include)
target_sources(drwlmtool PRIVATE
    src/drwlmtool.c)

target_link_libraries(drwlmtool drwlm-common)
