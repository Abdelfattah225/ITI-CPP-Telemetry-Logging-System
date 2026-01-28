load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "RingBuffer",
    hdrs = ["RingBuffer.hpp"],
    includes = ["."],
)

cc_library(
    name = "ThreadSafeRingBuffer",
    hdrs = ["ThreadSafeRingBuffer.hpp"],
    includes = ["."],
    deps = [":RingBuffer"],
)

cc_library(
    name = "async_logging_hdrs",
    hdrs = [
        "RingBuffer.hpp",
        "ThreadSafeRingBuffer.hpp",
    ],
    includes = ["."],
)