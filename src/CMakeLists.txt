add_executable( pacman pacman.cc )
add_executable( rockets rockets.cc )
add_executable( bsp bsp.cc )
add_executable( terain terain_generation.cc )

target_link_libraries( pacman raylib )
target_link_libraries( rockets raylib )
target_link_libraries( bsp raylib )
target_link_libraries( terain raylib )
