
add_executable(
    raycaster

    raycaster.cc
    texture_container.cc
    world_model.cc
    world_view.cc
    raylib_adapter.cc
)

target_link_libraries( raycaster raylib level_ds )

add_custom_target(copy_textures ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/textures
            ${CMAKE_CURRENT_BINARY_DIR}/textures
)

add_dependencies(raycaster copy_textures)

add_custom_target(copy_levels ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/levels
            ${CMAKE_CURRENT_BINARY_DIR}/levels
)

add_dependencies(raycaster copy_levels)
