
# add_dependencies(rayeditor editor_levels)
add_library(
    level_ds

    level.cc
    grid.cc
)

target_include_directories( level_ds INTERFACE ./ )

