#pragma once

#include <string>
#include <vector>

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

template <typename InputIterator1, typename InputIterator2,
          typename OutputIterator>
OutputIterator intersect(InputIterator1 first_begin, InputIterator1 first_end,
                         InputIterator2 second_begin, InputIterator2 second_end,
                         OutputIterator out) {
    while (first_begin != first_end && second_begin != second_end) {
        if (*first_begin == *second_begin) {
            *out = *first_begin;
            ++first_begin;
            ++second_begin;
            ++out;
        } else if (*first_begin < *second_begin) {
            ++first_begin;
        } else {
            ++second_begin;
        }
    }
    return out;
};