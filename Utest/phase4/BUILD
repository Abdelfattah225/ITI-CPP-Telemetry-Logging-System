load("@rules_cc//cc:defs.bzl", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_test(
    name = "RingBufferTest",
    srcs = ["RingBufferTest.cpp"],
    deps = [
        "//inc/AsyncLogging:RingBuffer",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "ThreadSafeRingBufferTest",
    srcs = ["ThreadSafeRingBufferTest.cpp"],
    deps = [
        "//inc/AsyncLogging:ThreadSafeRingBuffer",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "ThreadPoolTest",
    srcs = ["ThreadPoolTest.cpp"],
    deps = [
        "//src:async_logging",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "AsyncLogManagerTest",
    srcs = ["AsyncLogManagerTest.cpp"],
    deps = [
        "//src:async_logging",
        "//src:logging",
        "@googletest//:gtest_main",
    ],
)

test_suite(
    name = "phase4_tests",
    tests = [
        ":RingBufferTest",
        ":ThreadSafeRingBufferTest",
        ":ThreadPoolTest",
        ":AsyncLogManagerTest",
    ],
)