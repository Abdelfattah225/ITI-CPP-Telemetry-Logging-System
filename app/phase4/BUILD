load("@rules_cc//cc:defs.bzl", "cc_binary")

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "test_threadsafe_buffer",
    srcs = ["test_threadsafe_buffer.cpp"],
    deps = [
        "//inc/AsyncLogging:ThreadSafeRingBuffer",
    ],
)

cc_binary(
    name = "test_async_log_manager",
    srcs = ["test_async_log_manager.cpp"],
    deps = [
        "//src:async_logging",
        "//src:logging",
        "//inc/Formatter:formatter_hdrs",
    ],
)

cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    deps = [
        "//src:async_logging",
        "//src:logging",
        "//src:smart_data_hub",
    ],
)

cc_binary(
    name = "test_thread_pool",
    srcs = ["test_thread_pool.cpp"],
    deps = [
        "//src:async_logging",
    ],
)