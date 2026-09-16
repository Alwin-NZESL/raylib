cmake_minimum_required( VERSION 3.20 )
project( raylib )

set( CMAKE_CXX_STANDARD 20 )

add_subdirectory( vendor/raylib )
add_subdirectory( resources )
add_subdirectory( src )

add_subdirectory( raycaster )
add_subdirectory( bsppainter )

add_subdirectory( rayeditor )
