# app/phase3/BUILD
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")
cc_binary(
    name = "phase3_app",
    srcs = ["main.cpp"],
    deps = [
      
        "//src:formatter",   # This includes LogSinkFactory.cpp
        "//src:logging",     # This includes ConsoleSinkImpl.cpp, FileSinkImpl.cpp, etc.
    
    ],
)