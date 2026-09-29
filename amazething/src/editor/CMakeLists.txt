add_library(
    editor

    amazeditor.cc
    amazeview.cc
)

target_link_libraries( editor PRIVATE level_ds )
target_include_directories( editor INTERFACE ./ )

add_executable(
    rayeditor

    rayeditor.cc
    textboxwrapper.cc
    raylib_adapter.cc
)

target_link_libraries( rayeditor PRIVATE level_ds editor raylib )
