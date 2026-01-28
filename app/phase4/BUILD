load("@rules_cc//cc:defs.bzl", "cc_binary")

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "test_threadsafe_buffer",
    srcs = ["test_threadsafe_buffer.cpp"],
    deps = [
        "//inc/AsyncLogging:ThreadSafeRingBuffer",
    ],
)