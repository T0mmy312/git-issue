#include <nlohmann/json_fwd.hpp>
#include <commands.h>
#include <filesystem>
#include <iostream>
#include <cxxopts.hpp>
#include <util.h>
#include <stdexcept>
#include <commands.h>
#include <stdint.h>
#include <iostream>
#include <nlohmann/json.hpp>

int init_command(int argc, char** argv) {
    std::filesystem::path gitRoot;
    try {
        gitRoot = getGitRoot();
    } catch (std::runtime_error e) {
        std::cout << e.what() << std::endl;
        return -1;
    }

    // TODO: add the post-commit hook here to automatically close / refrence commits

    std::filesystem::create_directory(gitRoot / "issues");

    if (!std::filesystem::exists(gitRoot / "issues" / "free_issue_num.txt")) {
        try {
            setFreeIssueNumber(gitRoot, 1);
        } catch (std::runtime_error e) {
            std::cout << e.what() << std::endl;
            return -1;
        }
    }

    std::cout << "Initialized issues in " << gitRoot << std::endl;

    return 0;
}