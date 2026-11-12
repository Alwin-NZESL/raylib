cmake_minimum_required( VERSION 3.20 )
project( testbed VERSION 0.1.0 LANGUAGES CXX )

set( CMAKE_CXX_STANDARD 20 )

include_directories( vendor/raygui/src )

# Prevent Raylib's own CPack configuration from being included.
list(PREPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
)

add_subdirectory( vendor/raylib )

list(REMOVE_ITEM CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
)

add_subdirectory( resources )
add_subdirectory( src )
add_subdirectory( bsppainter )


set(CPACK_PACKAGE_NAME "testbed")
set(CPACK_PACKAGE_VERSION "0.1.0")
set(CPACK_GENERATOR "ZIP")

include(CPack)
