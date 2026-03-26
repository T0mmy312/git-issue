#include <commands.h>
#include <cstdint>
#include <cxxopts.hpp>
#include <fstream>
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
        std::vector<std::tuple<std::filesystem::path, uint64_t, std::string>> matches;
        for (const std::filesystem::directory_entry& file : iterator) {
            if (file.path().filename() == "free_issue_num.txt")
                continue;
            if (!file.is_regular_file())
                continue;
            std::ifstream fst(file.path());
            if (!fst.good() || !fst.is_open()) {
                fst.close();
                continue;
            }
            nlohmann::json data;
            try { data = nlohmann::json::parse(fst); } catch (...) { continue; }
            if (!data.contains("title") || !data.contains("issue_num") || !data.contains("desc"))
                continue;
            if (data["title"] == title)
                matches.push_back(std::tuple<std::filesystem::path, uint64_t, std::string>(file.path(), data["issue_num"], data["desc"]));
        }

        if (matches.size() == 0) {
            std::cout << "Could not find issue with title: '" << title << "'" << std::endl;
            return -1; 
        }
        else if (matches.size() > 1) {
            std::cout << "git issue remove matched multiple issues with the same title." << std::endl;
            std::cout << "Please remove the correct one using git issue remove -n <issue number>" << std::endl;
            for (const std::tuple<std::filesystem::path, uint64_t, std::string>& issue : matches) {
                std::cout << "Issue #" << std::get<1>(issue) << ":\n  ";
                for (char a : std::get<2>(issue)) {
                    if (a == '\n')
                        std::cout << "\n  ";
                    else
                        std::cout << a;
                }
                std::cout << std::endl;
            }
            return 0;
        }

        if (std::filesystem::remove_all(std::get<0>(matches[0]))) {
            std::cout << "Successfully removed issue #" << std::get<1>(matches[0]) << std::endl;
            return 0;
        } else {
            std::cout << "Could not remove issue #" << std::get<1>(matches[0]) << std::endl;
            return -1;
        }
    }

    return 0;
}