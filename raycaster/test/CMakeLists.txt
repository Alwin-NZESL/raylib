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

# add_executable(
#     test_raycaster

# )

# target_link_libraries( test_raycaster PRIVATE GTest::gtest_main level_ds )

# gtest_discover_tests( test_raycaster )
