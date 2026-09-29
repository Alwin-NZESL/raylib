
add_custom_target(copy_levels ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/levels
            ${CMAKE_CURRENT_BINARY_DIR}/levels
)

add_custom_target(copy_textures ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/textures
            ${CMAKE_CURRENT_BINARY_DIR}/textures
)
