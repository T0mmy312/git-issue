#include <cmd_util.h>
#include <reproc++/reproc.hpp>
#include <reproc++/run.hpp>
#include <stdexcept>
#include <utility>

std::string getGitRoot() {
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

    if (status.first != 0) {
        throw std::runtime_error(error.empty() ? "git failed" : error);
    }

    output.pop_back(); // simply remove the \n
    return output;
}