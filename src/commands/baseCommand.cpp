#include <nlohmann/json_fwd.hpp>
#include <commands.h>
#include <iostream>
#include <cxxopts.hpp>
#include <util.h>
#include <commands.h>
#include <string>
#include <stdint.h>
#include <iostream>
#include <nlohmann/json.hpp>

int base_command(int argc, char** argv) {
    cxxopts::Options options("git issue", "Local github style issue management.");
    options.add_options()
        ("h", "Prints this message")
        ("v,version", "Prints version information and exits");
    
    cxxopts::ParseResult result = options.parse(argc, argv);

    if (result.contains("h")) {
        std::cout << options.help() << std::endl;
        std::cout << "Sub Commands:\n  init\n  add\n  remove\n  close" << std::endl;
        return 0;
    }
    if (result.contains("version")) {
        std::cout << "git issue version: " << GIT_ISSUE_VERSION << std::endl;
        return 0;
    }
    std::cout << options.help() << std::endl;
    std::cout << "Sub Commands:\n  init\n  add\n  remove\n  close" << std::endl;
    return 0;
}