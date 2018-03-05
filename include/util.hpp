#pragma once

#include <vector>
#include <string>

std::vector<std::string> split(std::string& str, const std::string& delimeters);

template <typename T, typename InputIterator>
bool one_of(InputIterator begin, InputIterator end, const T& value) {
    for (auto it = begin; it != end; ++it) {
        if (*it == value) {
            return true;
        }
    }
    return false;
};