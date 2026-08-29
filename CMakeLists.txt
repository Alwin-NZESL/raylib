cmake_minimum_required( VERSION 3.20 )
project( raylib )

add_subdirectory( vendor/raylib )
add_subdirectory( resources )
add_subdirectory( src )

add_subdirectory( raycaster )

