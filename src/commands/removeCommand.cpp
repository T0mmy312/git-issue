#include "nlohmann/json_fwd.hpp"
#include <commands.h>
#include <cstdint>
#include <cxxopts.hpp>
#include <string>
#include <util.h>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>

int remove_command(int argc, char** argv) {
    cxxopts::Options options("git issue remove", "Remove a local issue.");
    options.add_options()
        ("h", "Prints this message")
        ("n,number", "Issue number to delete", cxxopts::value<uint64_t>())
        ("t,title", "Issue with given title to delete", cxxopts::value<std::string>());
    
    cxxopts::ParseResult result = options.parse(argc, argv);

    if (result.contains("h")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (!result.contains("number") && !result.contains("title")) {
        std::cout << "Fatal: git issue remove requires either number of title!" << std::endl;
        std::cout << options.help() << std::endl;
        return -1;
    }
    if (result.contains("number") && result.contains("title")) {
        std::cout << "Fatal: git issue remove may only take either number or title, not both!" << std::endl;
        return -1;
    }

    std::filesystem::path gitRoot;
    try {
        gitRoot = getGitRoot();
    } catch (std::runtime_error e) {
        std::cout << e.what() << std::endl;
        return -1;
    }

    if (result.contains("number")) {
        uint64_t issueNumber = result["number"].as<uint64_t>();
        std::filesystem::path issuePath = gitRoot / "issues" / (std::to_string(issueNumber) + ".json");
        if (!std::filesystem::exists(issuePath)) {
            std::cout << "Issue number does not exsist, nothing to remove." << std::endl;
            return 0;
        }
        if (std::filesystem::remove_all(issuePath)) {
            std::cout << "Successfully removed issue #" << issueNumber << std::endl;
            return 0;
        } else {
            std::cout << "Could not remove issue #" << issueNumber << std::endl;
            return -1;
        }
    }
    else {
        std::string title = result["title"].as<std::string>();
        std::filesystem::directory_iterator iterator(gitRoot / "issues");
        std::vector<std::pair<std::filesystem::path, nlohmann::json>> matches = getIssuesWithTitle(gitRoot, title);

        if (matches.size() == 0) {
            std::cout << "Could not find issue with title: '" << title << "'" << std::endl;
            return -1; 
        }
        else if (matches.size() > 1) {
            std::cout << "git issue remove matched multiple issues with the same title." << std::endl;
            std::cout << "Please remove the correct one using git issue remove -n <issue number>" << std::endl;
            for (const std::pair<std::filesystem::path, nlohmann::json>& issue : matches) {
                if (!issue.second.contains("issue_num") && !issue.second.contains("desc"))
                    continue;
                std::cout << "Issue #" << issue.second["issue_num"] << ":\n  ";
                for (char a : std::string(issue.second["desc"])) {
                    if (a == '\n')
                        std::cout << "\n  ";
                    else
                        std::cout << a;
                }
                std::cout << std::endl;
            }
            return 0;
        }

        if (std::filesystem::remove_all(matches[0].first)) {
            if (!matches[0].second.contains("issue_num")) {
                std::cout << "Successfully removed issue!" << std::endl;
                return 0;
            }
            std::cout << "Successfully removed issue #" << matches[0].second["issue_num"] << std::endl;
            return 0;
        } else {
            if (!matches[0].second.contains("issue_num")) {
                std::cout << "Could not remove issue!";
                return -1;
            }
            std::cout << "Could not remove issue #" << matches[0].second["issue_num"] << std::endl;
            return -1;
        }
    }

    return 0;
}