#include <cstring>
#include "util.hpp"

std::vector<std::string> split(std::string& str,
                               const std::string& delimeters) {
    std::vector<std::string> result;

    char* token = strtok(&str[0], delimeters.c_str());
    while (token != nullptr) {
        result.emplace_back(token);
        token = strtok(nullptr, delimeters.c_str());
    }

    return result;
}
