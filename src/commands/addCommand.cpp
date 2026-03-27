#include <commands.h>
#include <nlohmann/json_fwd.hpp>
#include <commands.h>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cxxopts.hpp>
#include <util.h>
#include <stdexcept>
#include <commands.h>
#include <string>
#include <stdint.h>
#include <iostream>
#include <nlohmann/json.hpp>

int add_command(int argc, char** argv) {
    cxxopts::Options options("git issue add", "Adds an inssue to the local project.");
    options.add_options()
        ("h", "Prints this message")
        ("t,title", "Title of the issue", cxxopts::value<std::string>())
        ("d,desc", "The description of the issue", cxxopts::value<std::string>()->default_value(""))
        ("i,input", "Uses the default git editor to get inputs for the desc and title!");
    
    cxxopts::ParseResult result = options.parse(argc, argv);

    if (result.contains("h")) {
        std::cout << options.help() << std::endl;
        return 0;
    }
    
    if (!result.contains("title") && !result.contains("input")) {
        std::cout << "Fatal: command 'git issue add' requires a title!" << std::endl;
        std::cout << options.help() << std::endl;
        return -1;
    }

    std::filesystem::path gitRoot;
    try {
        gitRoot = getGitRoot();
    } catch (std::runtime_error e) {
        std::cout << e.what() << std::endl;
        return -1;
    }

    uint64_t issueNumber = 1;
    try {
        issueNumber = getFreeIssueNumber(gitRoot);
    } catch (std::runtime_error e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
    
    std::filesystem::path issueFilePath = gitRoot / "issues" / (std::to_string(issueNumber) + ".json"); 
    if (std::filesystem::exists(issueFilePath)) {
        if (issueNumber + 1 < issueNumber) {
            std::cout << "Fatal: No more issues possible too many issues (interger overflow)!" << std::endl;
            return -1;
        }
        issueNumber++;
        while (std::filesystem::exists(gitRoot / "issues" / (std::to_string(issueNumber) + ".json"))) {
            if (issueNumber + 1 < issueNumber) {
                std::cout << "Fatal: No more issues possible too many issues (interger overflow)!" << std::endl;
                return -1;
            }
            issueNumber++;
        }
        try {
            if (issueNumber + 1 < issueNumber) {
                std::cout << "Fatal: No more issues possible too many issues (interger overflow)!" << std::endl;
                return -1;
            }
            setFreeIssueNumber(gitRoot, issueNumber + 1);
        } catch (std::runtime_error e) {
            std::cout << e.what() << std::endl;
            return -1;
        }
        issueFilePath = gitRoot / "issues" / (std::to_string(issueNumber) + ".json");
    }

    nlohmann::json issue;
    
    if (result.contains("input") && !result.contains("title"))
        issue["title"] = getInput(gitRoot, "# lines with # as the first char will be ignored\n# Please input the title for your issue:");
    else
        issue["title"] = result["title"].as<std::string>();

    if (result.contains("input") && !result.contains("desc"))
        issue["desc"] = getInput(gitRoot, "# lines with # as the first char will be ignored\n# Please input the description for your issue:");
    else
        issue["desc"] = result["desc"].as<std::string>();

    issue["issue_num"] = issueNumber;
    issue["open"] = true;
    issue["commit_hashes"] = {};

    std::ofstream file(issueFilePath);
    if (!file.good() || !file.is_open()) {
        std::cout << "Fatal: Could not create issue file!" << std::endl;
        file.close();
        return -1;
    }
    try {
        file << issue;
    } catch (std::exception e) {
        std::cout << "Fatal: could not write to issue file!" << std::endl;
        file.close();
        return -1;
    }
    file.close();

    std::cout << "added issue #" << issueNumber << ": " << issue["title"] << std::endl;

    return 0;
}