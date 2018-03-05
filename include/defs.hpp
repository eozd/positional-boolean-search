#pragma once

#include <string>
#include <unordered_map>
#include <vector>

using raw_doc = std::string;
using raw_doc_index = std::unordered_map<size_t, raw_doc>;
using doc_term_index = std::unordered_map<size_t, std::vector<std::string>>;
using pos_inv_index =
    std::unordered_map<std::string,
                       std::vector<std::pair<size_t, std::vector<size_t>>>>;

using term_id_map = std::unordered_map<std::string, size_t>;
using id_pos_map =
    std::unordered_map<size_t,
                       std::vector<std::pair<size_t, std::vector<size_t>>>>;
