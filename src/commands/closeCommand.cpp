#include <nlohmann/json_fwd.hpp>
#include <commands.h>
#include <cxxopts.hpp>
#include <exception>
#include <fstream>
#include <ostream>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <iostream>
#include <util.h>
#include <vector>
#include <filesystem>

int close_command(int argc, char** argv) {
    cxxopts::Options options("git issue close", "Closes a local issue.");
    options.add_options()
        ("h", "Prints this message")
        ("n,number", "Issue number to close", cxxopts::value<uint64_t>())
        ("t,title", "Issue with given title to delete", cxxopts::value<std::string>());
    
    cxxopts::ParseResult result = options.parse(argc, argv);

    if (result.contains("-h")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (!result.contains("title") && !result.contains("number")) {
        std::cout << "Fatal: git issue close requires either a title or an issue number!" << std::endl;
        std::cout << options.help() << std::endl;
        return -1;
    }
    if (result.contains("title") && result.contains("number")) {
        std::cout << "Fatal: git issue close requires either only a title or a only number, not both!" << std::endl;
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
            std::cout << "Issue number does not exsist, nothing to close." << std::endl;
            return 0;
        }
        
        nlohmann::json data;
        try {
            data = getJson(issuePath);
        } catch (std::runtime_error e) {
            std::cout << e.what() << std::endl;
            return -1;
        }

        if (!data.contains("open")) {
            std::cout << "Fatal: Issue does not contain a \"open\" value!" << std::endl;
            return -1;
        }
        if (!data["open"]) {
            std::cout << "Issue #" << issueNumber << " " << std::endl;
        }
        data["open"] = false;

        std::ofstream issueFile(issuePath);
        if (!issueFile.is_open() || !issueFile.good()) {
            std::cout << "Fatal: could not write to issue file!" << std::endl;
            return -1;
        }
        issueFile << data;
        issueFile.close();
        std::cout << "Successfully closed issue #" << issueNumber << "!" << std::endl;
        return 0;
    }

    if (result.contains("title")) {
        std::vector<std::pair<std::filesystem::path, nlohmann::json>> issues = getIssuesWithTitle(gitRoot, result["title"].as<std::string>());

        for (std::pair<std::filesystem::path, nlohmann::json>& issue : issues) {
            if (!issue.second.contains("open")) {
                std::cout << "Fatal: Issue #" << issue.second["issue_num"] << " does not contain an \"open\" value!" << std::endl;
                continue;
            }
            issue.second["open"] = false;
            std::ofstream issueFile(issue.first);
            if (!issueFile.is_open() || !issueFile.good()) {
                std::cout << "Fatal: could not write to issue file " << issue.first << "!" << std::endl;
                continue;
            }
            issueFile << issue.second;
            issueFile.close();
            std::cout << "Successfully closed issue #" << issue.second["issue_num"] << "!" << std::endl;
        }

        return 0;
    }

    return 0;
}