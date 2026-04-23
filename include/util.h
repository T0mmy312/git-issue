#ifndef UTIL_H
#define UTIL_H

#include "nlohmann/json_fwd.hpp"
#include <cstdint>
#include <stdint.h>
#include <filesystem>
#include <vector>
#include <nlohmann/json.hpp>

std::filesystem::path getGitRoot();
uint64_t getFreeIssueNumber(std::filesystem::path gitRoot);
void setFreeIssueNumber(std::filesystem::path gitRoot, uint64_t number);
std::string getInput(std::filesystem::path gitRoot, std::string prompt);
std::vector<std::pair<std::filesystem::path, nlohmann::json>> getIssuesWithTitle(std::filesystem::path gitRoot, std::string title);
nlohmann::json getJson(std::filesystem::path file);

#endif // UTIL_H