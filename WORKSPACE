workspace(name = "bzlx")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "bazel_skylib",
    sha256 = "74d544d96f4a5bb630d465ca8bbcfe231e3594e5aae57e1edbf17a6eb3ca2506",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.3.0/bazel-skylib-1.3.0.tar.gz",
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.3.0/bazel-skylib-1.3.0.tar.gz",
    ],
)

load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")

bazel_skylib_workspace()

http_archive(
    name = "boost",
    sha256 = "4a79c389add7e3d54d0e12c83098d471d24536ba2d6b8593d3a95f151f25eebb",
    strip_prefix = "boost-da62319c330d81ef032517cbe13b6f35d97387cb",
    urls = ["https://github.com/bazelboost/boost/archive/da62319c330d81ef032517cbe13b6f35d97387cb.zip"],
)

load("@boost//:index.bzl", "boost_http_archives")

boost_http_archives()

http_archive(
    name = "hedron_compile_commands",
    sha256 = "4b251a482a85de6c5cb0dc34c5671e73190b9ff348e9979fa2c033d81de0f928",
    strip_prefix = "bazel-compile-commands-extractor-5bb5ff2f32d542a986033102af771aa4206387b9",
    url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/5bb5ff2f32d542a986033102af771aa4206387b9.tar.gz",
)

load("@hedron_compile_commands//:workspace_setup.bzl", "hedron_compile_commands_setup")

hedron_compile_commands_setup()

http_archive(
    name = "com_grail_bazel_toolchain",
    sha256 = "7fa5a8624b1148c36e09c7fa29ef6ee8b83f865219c9c219c9125aac78924758",
    strip_prefix = "bazel-toolchain-c3131a6894804ee586d059c57ffe8e88d44172e1",
    url = "https://github.com/grailbio/bazel-toolchain/archive/c3131a6894804ee586d059c57ffe8e88d44172e1.zip",
)

load("@com_grail_bazel_toolchain//toolchain:deps.bzl", "bazel_toolchain_dependencies")

bazel_toolchain_dependencies()

load("@com_grail_bazel_toolchain//toolchain:rules.bzl", "llvm_toolchain")

llvm_toolchain(
    name = "llvm_toolchain",
    cxx_standard = {"linux": "c++20"},
    llvm_version = "14.0.0",
)

load("@llvm_toolchain//:toolchains.bzl", "llvm_register_toolchains")

llvm_register_toolchains()
