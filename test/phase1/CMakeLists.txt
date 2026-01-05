# Fetch Google Test (remove find_package - use only FetchContent)
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Create test executable (NO main.cpp - gtest_main provides it!)
add_executable(phase1_tests
    LogMessageTest.cpp
    ConsoleSinkTest.cc
    FileSinkTest.cc
    LogManagerTest.cc
)

# Add include directories
target_include_directories(phase1_tests PRIVATE
    ${PROJECT_SOURCE_DIR}/inc
)

# Link libraries (order matters!)
target_link_libraries(phase1_tests PRIVATE
    logging
    gtest
    gtest_main
)

# Register tests
include(GoogleTest)
gtest_discover_tests(phase1_tests)