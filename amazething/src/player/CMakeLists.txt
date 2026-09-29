add_library(
    player

    world_model.cc
    world_view.cc
)

target_link_libraries( player level_ds raylib )

add_executable(
    raycaster

    raycaster.cc
    texture_container.cc
    raylib_adapter.cc
)

target_link_libraries( raycaster level_ds player raylib )

