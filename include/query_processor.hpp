#pragma once

#include <vector>
#include <string>


std::vector<size_t> conjunctive_query(const std::vector<std::string>& words);
std::vector<size_t> phrase_query(const std::vector<std::string>& words);
std::vector<size_t> proximity_query(const std::vector<std::string>& words,
                                    const std::vector<size_t>& dists);
