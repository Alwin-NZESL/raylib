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

add_executable(
    test_editor

    test_level.cc
    test_grid.cc
    test_amazeditor.cc
    test_amazeview.cc
    mock_adapter.cc
    mock_textboxwrapper.cc
    test_editor_view_int.cc
)

target_compile_definitions( test_editor PRIVATE TEST_LEVEL_DIR="${CMAKE_SOURCE_DIR}/levels" )

target_link_libraries( test_editor PRIVATE GTest::gtest_main level_ds )

gtest_discover_tests( test_editor )
