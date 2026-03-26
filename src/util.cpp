#include <util.h>
#include <reproc++/reproc.hpp>
#include <reproc++/run.hpp>
#include <stdexcept>
#include <utility>
#include <fstream>

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
        throw std::runtime_error(error.empty() ? "git failed" : error);

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