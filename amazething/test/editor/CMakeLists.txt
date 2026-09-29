
add_executable(
    test_editor

    test_amazeditor.cc
    test_amazeview.cc
    mock_adapter.cc
    mock_textboxwrapper.cc
    test_editor_view_int.cc
)

target_link_libraries( test_editor PRIVATE GTest::gtest_main editor level_ds )

gtest_discover_tests( test_editor )
