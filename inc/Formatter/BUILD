load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")
package(default_visibility = ["//visibility:public"])


cc_library(
    name = "formatter_hdrs",
    hdrs = [
       "Enums.hpp",
       "Policies.hpp",
       "Parser.hpp",
       "magic_enum.hpp",
       "LogSinkFactory.hpp",
       "LogManagerBuilder.hpp",
       "LogFormatter.hpp"
    ],

    include_prefix = "",
    strip_include_prefix = "",
    includes = ["."],  # Adds inc/logging to include path
      deps = [
        "//inc/logging:logging_hdrs",  # Add dependency on logging headers
    ],
)




# for testing 
cc_library(
    name = "enums",
    hdrs = ["Enums.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        "@magic_enum//:magic_enum",
    ],
    copts = ["-std=c++17"],
)

cc_library(
    name = "policies",
    hdrs = ["Policies.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        ":enums",
    ],
    copts = ["-std=c++17"],
)

cc_library(
    name = "parser",
    hdrs = ["Parser.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        ":policies",
        ":enums",
    ],
    copts = ["-std=c++17"],
)

cc_library(
    name = "log_formatter",
    hdrs = ["LogFormatter.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        ":parser",
        ":policies",
        ":enums",
        "//inc/logging:log_message",
        "@magic_enum//:magic_enum",
    ],
    copts = ["-std=c++17"],
)

cc_library(
    name = "log_sink_factory",
    hdrs = ["LogSinkFactory.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        ":enums",
        "//inc/logging:log_sink",
    ],
    copts = ["-std=c++17"],
)

cc_library(
    name = "log_manager_builder",
    hdrs = ["LogManagerBuilder.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        ":enums",
        ":log_sink_factory",
        "//inc/logging:log_manager",
    ],
    copts = ["-std=c++17"],
)