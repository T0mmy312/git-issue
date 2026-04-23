#include <reproc++/arguments.hpp>
#include <reproc++/drain.hpp>
#include <filesystem>
#include <system_error>
#include <util.h>
#include <reproc++/reproc.hpp>
#include <reproc++/run.hpp>
#include <stdexcept>
#include <utility>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

std::filesystem::path getGitRoot() {
    std::string output;
    std::string error;

    reproc::options opts = {};
    std::vector<std::string> argv = {
        "git",
        "rev-parse",
        "--show-toplevel"
    };
    reproc::arguments args(argv);
    std::pair<int, std::error_code> status = reproc::run(
        args,
        opts,
        reproc::sink::string(output),
        reproc::sink::string(error)
    );

    if (status.first != 0)
        throw std::runtime_error(error.empty() ? "Fatal: git failed" : error);

    output.pop_back(); // simply remove the \n

    std::filesystem::path gitRoot(output);
    gitRoot /= ".git";
    if (!std::filesystem::is_directory(gitRoot))
        throw std::runtime_error("Fatal: can't find the .git directory!");

    return gitRoot;
}

uint64_t getFreeIssueNumber(std::filesystem::path gitRoot) {
    std::ifstream issueNumFile(gitRoot / "issues" / "free_issue_num.txt");
    if (!issueNumFile.good() || !issueNumFile.is_open()) {
        issueNumFile.close();
        throw std::runtime_error("Fatal: Could not read issue number file!");
    }

    uint64_t issueNumber = 1;
    try {
        issueNumFile >> issueNumber;
    } catch (std::exception e) {
        throw std::runtime_error("Fatal: Could not read issue number from issue number file!");
    };
    issueNumFile.close();

    std::ofstream issueNumFileOut(gitRoot / "issues" / "free_issue_num.txt");
    if (!issueNumFileOut.good() || !issueNumFileOut.is_open()) {
        issueNumFileOut.close();
        throw std::runtime_error("Fatal: Could not write to issue number file!");
    }

    if ((issueNumber + 1) < issueNumber)
        throw std::runtime_error("Fatal: too many issues (integer overflow)!");
    issueNumFileOut << issueNumber + 1;
    issueNumFileOut.close();

    return issueNumber;
}


void setFreeIssueNumber(std::filesystem::path gitRoot, uint64_t number) {
    std::ofstream issueNumFile(gitRoot / "issues" / "free_issue_num.txt");
    if (!issueNumFile.good() || !issueNumFile.is_open()) {
        issueNumFile.close();
        throw std::runtime_error("Fatal: Could not create / write to issue number file!");
    }
    issueNumFile << number;
    issueNumFile.close();
}

std::string getInput(std::filesystem::path gitRoot, std::string prompt) {
    std::ofstream tempFile(gitRoot / "GIT_ISSUE_TEMP_INPUT.txt");
    if (!tempFile.good() || !tempFile.is_open()) {
        tempFile.close();
        throw std::runtime_error("Fatal: Could not create / write to .git/GIT_ISSUE_TEMP_INPUT.txt!");
    }
    tempFile << prompt;
    tempFile.close();

    // git var GIT_EDITOR
    std::string output;
    std::string error;

    reproc::options opts = {};
    std::vector<std::string> argv = {
        "git",
        "var",
        "GIT_EDITOR"
    };
    reproc::arguments args(argv);
    std::pair<int, std::error_code> status = reproc::run(
        args,
        opts,
        reproc::sink::string(output),
        reproc::sink::string(error)
    );

    if (status.first != 0)
        throw std::runtime_error(error.empty() ? "Fatal: could not get git's default editor!" : error);

    output.pop_back(); // remove the \n

    opts.redirect.parent = true; // this makes the terminal executing it not my c++ program
    argv = {
        output,
        gitRoot / "GIT_ISSUE_TEMP_INPUT.txt"
    };
    args = reproc::arguments(argv);
    status = reproc::run(
        args,
        opts,
        reproc::sink::string(output),
        reproc::sink::string(error)
    );

    if (status.first != 0)
        throw std::runtime_error(error.empty() ? "Fatal: could not call default editor!" : error);

    std::ifstream tempFileRead(gitRoot / "GIT_ISSUE_TEMP_INPUT.txt");
    if (!tempFileRead.good() || !tempFileRead.is_open()) {
        tempFileRead.close();
        throw std::runtime_error("Fatal: could not read .git/GIT_ISSUE_TEMP_INPUT.txt!");
    }
    std::stringstream ss;
    std::string line;
    while (std::getline(tempFileRead, line)) {
        if (line.size() >= 1)
            if (line[0] == '#')
                continue;
        ss << line;
    }
    tempFileRead.close();
    std::filesystem::remove_all(gitRoot / "GIT_ISSUE_TEMP_INPUT.txt");
    return ss.str();
}

std::vector<std::pair<std::filesystem::path, nlohmann::json>> getIssuesWithTitle(std::filesystem::path gitRoot, std::string title) {
    std::filesystem::directory_iterator iterator(gitRoot / "issues");
    std::vector<std::pair<std::filesystem::path, nlohmann::json>> matches;
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
        if (!data.contains("title"))
            continue;
        if (data["title"] == title)
            matches.push_back(std::pair<std::filesystem::path, nlohmann::json>(file.path(), data));
    }
    return matches;
}

nlohmann::json getJson(std::filesystem::path file) {
    if (!std::filesystem::exists(file))
        throw std::runtime_error("Fatal: given file does not exsist!");

    std::ifstream jsonFile(file);
    if (!jsonFile.is_open() || !jsonFile.good()) {
        jsonFile.close();
        throw std::runtime_error("Fatal: Could not open " + (std::string)file + "!");
    }
    nlohmann::json data;
    try { data = nlohmann::json::parse(jsonFile); }
    catch (std::exception e) {
        throw std::runtime_error("Fatal: json parse error: " + std::string(e.what()));
    }
    jsonFile.close();

    return data;
}