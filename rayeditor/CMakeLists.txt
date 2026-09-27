
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# Prevent GoogleTest from overriding our compiler/linker settings on Windows.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

add_library(
    level_ds

    level.cc
    grid.cc
    amazeditor.cc
    amazeview.cc
)

target_include_directories( level_ds INTERFACE ./ )


add_executable(
    rayeditor

    rayeditor.cc
    textboxwrapper.cc
    raylib_adapter.cc
)

add_executable(
    test_editor

    test_level.cc
    test_grid.cc
    test_amazeditor.cc
    test_amazeview.cc
)

target_link_libraries( rayeditor PRIVATE level_ds raylib )
target_link_libraries(test_editor PRIVATE GTest::gtest_main level_ds )

gtest_discover_tests(test_editor)
