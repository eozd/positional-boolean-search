#pragma once

#include <string>
#include <vector>
#include <unordered_map>

using raw_doc = std::string;
using raw_doc_index = std::unordered_map<size_t, raw_doc>;
using doc_term_index = std::unordered_map<size_t, std::vector<std::string>>;
