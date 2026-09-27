
add_subdirectory(src)
add_subdirectory(test)

add_custom_target(editor_levels ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/levels
            ${CMAKE_CURRENT_BINARY_DIR}/levels
)

add_dependencies(rayeditor editor_levels)
