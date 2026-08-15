add_executable( pacman pacman.cc )
add_executable( rockets rockets.cc )
add_executable( bsp bsp.cc )

target_link_libraries( pacman raylib )
target_link_libraries( rockets raylib )
target_link_libraries( bsp raylib )