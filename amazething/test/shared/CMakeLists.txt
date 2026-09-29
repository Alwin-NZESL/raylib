add_executable(
    test_shared

    test_level.cc
    test_grid.cc
)

target_link_libraries( test_shared PRIVATE GTest::gtest_main level_ds )

gtest_discover_tests( test_shared )
