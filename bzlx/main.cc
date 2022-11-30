#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

struct bazel_label_info {
	std::string target_name;
	std::string package_name;
	std::string workspace_name;
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
	// TODO
	return false;
}

auto run_workspace_module(
	const bazel_label_info&         label,
	const std::vector<std::string>& args
) -> int {
	// TODO
	return 0;
}

auto download_global_module(const bazel_label_info& label) -> int {
	// TODO
	return 0;
}

auto run_global_module(
	const bazel_label_info&         label,
	const std::vector<std::string>& args
) -> int {
	// TODO
	return 0;
}

auto main(int argc, char* argv[], char** envp) -> int {
	if(argc < 2) {
		std::cerr << "[ERROR] expected bazel label as first argument\n";
		return 1;
	}

	auto label = parse_label_string(argv[1]);

	std::cout //
		<< "workspace_name: " << label.workspace_name << "\n"
		<< "package_name: " << label.package_name << "\n"
		<< "target_name: " << label.target_name << "\n";

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

	if(auto exit_code = download_global_module(label); exit_code != 0) {
		return exit_code;
	}

	return run_global_module(label, run_args);
}
