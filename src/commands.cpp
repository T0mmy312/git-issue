#include <commands.h>
#include <filesystem>
#include <iostream>
#include <cxxopts.hpp>
#include <cmd_util.h>
#include <stdexcept>
#include <commands.h>

int base_command(int argc, char** argv) {
    cxxopts::Options options("git issue", "Local github style issue management.");
    options.add_options()
        ("h", "Prints this message")
        ("v,version", "Prints version information and exits");
    
    cxxopts::ParseResult result = options.parse(argc, argv);

    if (result.contains("h")) {
        std::cout << options.help() << std::endl;
        std::cout << "Sub Commands:\n  init" << std::endl;
        return 0;
    }
    if (result.contains("version")) {
        std::cout << "git issue version: " << GIT_ISSUE_VERSION << std::endl;
        return 0;
    }
    std::cout << options.help() << std::endl;
    std::cout << "Sub Commands:\n  init" << std::endl;
    return 0;
}

int init_command(int argc, char** argv) {
    std::filesystem::path gitRoot;
    try {
        gitRoot = getGitRoot();
        gitRoot /= ".git";
    } catch (std::runtime_error e) {
        std::cout << e.what() << std::endl;
    }
    if (!std::filesystem::is_directory(gitRoot)) {
        std::cout << "Fatal: Is not a git repository!" << std::endl;
    }

    // TODO: add the post-commit hook here to automatically close / refrence commits

    std::filesystem::create_directory(gitRoot / "issues");
    return 0;
}

int add_command(int argc, char** argv) {
    return 0;
}

int remove_command(int argc, char** argv) {
    return 0;
}

int close_command(int argc, char** argv) {
    return 0;
}