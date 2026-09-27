add_library(
    level_ds

    level.cc
    grid.cc
    amazeditor.cc
    amazeview.cc
)

target_include_directories( level_ds INTERFACE ./ )


add_executable(
    rayeditor

    rayeditor.cc
    textboxwrapper.cc
    raylib_adapter.cc
)

target_link_libraries( rayeditor PRIVATE level_ds raylib )
