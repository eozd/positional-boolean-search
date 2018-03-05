#pragma once

#include <vector>
#include <string>


std::vector<size_t> conjunctive_query(const std::string& query_str);
std::vector<size_t> phrase_query(const std::string& query_str);
std::vector<size_t> proximity_query(const std::string& query_str);
