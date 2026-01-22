# Utest/phase3/BUILD

load("@rules_cc//cc:defs.bzl", "cc_test")

# ============== Enums Tests ==============
cc_test(
    name = "enums_test",
    srcs = ["Enum_test.cpp"],
    deps = [
        "//inc/Formatter:enums",
        "@googletest//:gtest",           # ← Changed
        "@googletest//:gtest_main",      # ← Changed
        "@magic_enum//:magic_enum",
    ],
    copts = ["-std=c++17"],
)

# ============== Policies Tests ==============
cc_test(
    name = "policies_test",
    srcs = ["Policies_test.cpp"],
    deps = [
        "//inc/Formatter:policies",
        "//inc/Formatter:enums",
        "@googletest//:gtest",           # ← Changed
        "@googletest//:gtest_main",      # ← Changed
    ],
    copts = ["-std=c++17"],
)

# ============== Parser Tests ==============
cc_test(
    name = "parser_test",
    srcs = ["Parser_test.cpp"],
    deps = [
        "//inc/Formatter:parser",
        "//inc/Formatter:policies",
        "//inc/Formatter:enums",
        "@googletest//:gtest",           # ← Changed
        "@googletest//:gtest_main",      # ← Changed
    ],
    copts = ["-std=c++17"],
)

# ============== LogFormatter Tests ==============
cc_test(
    name = "log_formatter_test",
    srcs = ["LogFormatterTtest.cpp"],
    deps = [
        "//inc/Formatter:log_formatter",
        "//inc/Formatter:policies",
        "//inc/Formatter:enums",
        "//inc/logging:log_message",
        "@googletest//:gtest",           # ← Changed
        "@googletest//:gtest_main",      # ← Changed
        "@magic_enum//:magic_enum",
    ],
    copts = ["-std=c++17"],
)

# ============== LogSinkFactory Tests ==============
cc_test(
    name = "log_sink_factory_test",
    srcs = ["LogSinkFactory_test.cpp"],
    deps = [
        "//inc/Formatter:log_sink_factory",
        "//inc/Formatter:enums",
        "//inc/logging:log_sink",
        "@googletest//:gtest",           # ← Changed
        "@googletest//:gtest_main",      # ← Changed
    ],
    copts = ["-std=c++17"],
)

# ============== LogManagerBuilder Tests ==============
cc_test(
    name = "log_manager_builder_test",
    srcs = ["LogManagerBuilderTest.cpp"],
    deps = [
        "//inc/Formatter:log_manager_builder",
        "//inc/Formatter:enums",
        "//inc/logging:log_manager",
        "@googletest//:gtest",           # ← Changed
        "@googletest//:gtest_main",      # ← Changed
    ],
    copts = ["-std=c++17"],
)

# ============== Run All Tests ==============
test_suite(
    name = "phase3_tests",
    tests = [
        ":enums_test",
        ":policies_test",
        ":parser_test",
        ":log_formatter_test",
        ":log_sink_factory_test",
        ":log_manager_builder_test",
    ],
)