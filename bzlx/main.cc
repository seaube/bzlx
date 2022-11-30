#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <filesystem>
#include <fstream>
#include <boost/process.hpp>

namespace fs = std::filesystem;
namespace bp = boost::process;
using namespace std::string_literals;

constexpr auto GLOBAL_MODULE_CONTENTS = R"(
module(
    name = "bzlx_global_module",
    version = "0.1.0",
)
)";

struct bazel_label_info {
	std::string target_name;
	std::string package_name;
	std::string workspace_name;

	std::string to_string() const {
		return "@" + workspace_name + "//" + package_name + ":" + target_name;
	}
};

bazel_label_info parse_label_string(const std::string& label_str) {
	bazel_label_info info;

	auto ws_name_end = label_str.find("//");

	if(ws_name_end == std::string::npos && label_str.starts_with('@')) {
		info.workspace_name = label_str.substr(1);
	} else if(ws_name_end != std::string::npos) {
		info.workspace_name = label_str.substr(1, ws_name_end - 1);
		info.package_name = label_str.substr(ws_name_end + 2);
	}

	auto target_name_begin = info.package_name.find(':');
	if(target_name_begin != std::string::npos) {
		info.target_name = info.package_name.substr(target_name_begin + 1);
		info.package_name = info.package_name.substr(0, target_name_begin);
	}

	if(info.target_name.empty() && !info.package_name.empty()) {
		auto slash_idx = info.package_name.find('/');
		if(slash_idx == std::string::npos) {
			info.target_name = info.package_name;
		} else {
			info.target_name = info.package_name.substr(slash_idx + 1);
		}
	}

	if(!info.workspace_name.empty() && info.target_name.empty()) {
		info.target_name = info.workspace_name;
	}

	return info;
}

auto module_in_local_workspace(const bazel_label_info& label) -> bool {
	auto bazel = bp::search_path("bazel");

	bp::child child(
		bp::exe(bazel),
		bp::args({
			"query"s,
			"--ui_event_filters=-info,-stdout,-stderr"s,
			"--noshow_progress"s,
			label.to_string(),
		})
	);
	child.wait();

	auto exit_code = child.exit_code();
	if(exit_code != 0) {
		return false;
	}

	return true;
}

auto run_workspace_module(
	const bazel_label_info&         label,
	const std::vector<std::string>& args
) -> int {
	auto bazel = bp::search_path("bazel");

	const std::array fixed_args{
		"run"s,
		"--ui_event_filters=-info,-stdout,-stderr"s,
		"--noshow_progress"s,
		label.to_string(),
		" -- "s,
	};

	std::vector<std::string> run_args;
	run_args.reserve(args.size() + fixed_args.size());
	run_args.insert(run_args.end(), fixed_args.begin(), fixed_args.end());
	run_args.insert(run_args.end(), args.begin(), args.end());

	return bp::system(bp::exe(bazel), bp::args(run_args));
}

auto download_global_module(const bazel_label_info& label, const fs::path& dir)
	-> int {
	if(!fs::exists(dir)) {
		fs::create_directories(dir);
	}

	std::ofstream module_bzl(dir / "MODULE.bazel");

	// TODO(zaucy): get version
	std::string dep_version = "";
	if(label.workspace_name == "gazelle") {
		dep_version = "0.28.0";
	}

	module_bzl //
		<< GLOBAL_MODULE_CONTENTS
		<< "bazel_dep("
		<< "name=\"" << label.workspace_name << "\", "
		<< "version=\"" << dep_version << "\")\n";

	return 0;
}

auto run_global_module(
	const bazel_label_info&         label,
	const std::vector<std::string>& args,
	fs::path                        dir
) -> int {
	auto bazel = bp::search_path("bazel");
	return bp::system(
		bp::exe(bazel),
		bp::start_dir(dir.string()),
		bp::args({"run"s, label.to_string()})
	);
}

auto find_global_module_directory() -> fs::path {
	fs::path home_dir;
#if _WIN32
	home_dir = std::getenv("USERPROFILE");
#else
	home_dir = std::getenv("HOME");
#endif
	return home_dir / ".bzlx/";
}

auto main(int argc, char* argv[], char** envp) -> int {
	if(argc < 2) {
		std::cerr << "[ERROR] expected bazel label as first argument\n";
		return 1;
	}

	auto label = parse_label_string(argv[1]);

	if(label.workspace_name.empty()) {
		std::cerr //
			<< "[ERROR] bazel label must have module name\n\n"
			<< "        examples: bzlx @example\n"
			<< "                  bzlx @example//package/path\n"
			<< "                  bzlx @example//package/path:target\n"
			<< "\n";
		return 1;
	}

	std::vector<std::string> run_args;
	if(argc - 2 > 0) {
		run_args.reserve(argc - 2);
		for(int i = 2; argc > i; ++i) {
			run_args.push_back(argv[i]);
		}
	}

	if(module_in_local_workspace(label)) {
		return run_workspace_module(label, run_args);
	}

	auto global_module_dir = find_global_module_directory();

	if(auto exit_code = download_global_module(label, global_module_dir);
		 exit_code != 0) {
		return exit_code;
	}

	return run_global_module(label, run_args, global_module_dir);
}
