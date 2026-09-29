
add_executable(
    raycaster

    raycaster.cc
    texture_container.cc
    world_model.cc
    world_view.cc
    raylib_adapter.cc
)

target_link_libraries( raycaster raylib level_ds )

