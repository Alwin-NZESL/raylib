add_executable(
    bsppainter

    bsppainter.cc
    titlestate.cc
    renderer.cc
    menustate.cc
    controls.cc
    playstate.cc
)

target_link_libraries( bsppainter raylib )
