#ifndef UTIL_H
#define UTIL_H

#include <cstdint>
#include <string>
#include <stdint.h>
#include <filesystem>

std::filesystem::path getGitRoot();
uint64_t getFreeIssueNumber(std::filesystem::path gitRoot);
void setFreeIssueNumber(std::filesystem::path gitRoot, uint64_t number);

#endif // UTIL_H