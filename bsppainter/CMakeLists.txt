add_executable(
    bsppainter

    bsppainter.cc
    titlestate.cc
    renderer.cc
    menustate.cc
    controls.cc
    playstate.cc
    bspnode.cc
)

target_link_libraries( bsppainter raylib )

add_subdirectory( resources )
