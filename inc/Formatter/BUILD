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