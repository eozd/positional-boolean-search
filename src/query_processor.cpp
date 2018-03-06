/*
 * Copyright 2018 Esref Ozdemir
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "query_processor.hpp"
#include "util.hpp"
#include <algorithm>
#include <cassert>

ir::QueryProcessor::QueryProcessor(const term_id_map& dict,
                                   const id_pos_map& index)
    : m_dict(dict), m_index(index) {}

std::vector<size_t> ir::QueryProcessor::conjunctive_query(
    const std::vector<std::string>& words) const {

    // store document IDs of each term
    std::vector<std::vector<size_t>> posting_lists;
    for (const auto& word : words) {
        // if one of the terms doesn't appear at all, return empty set.
        if (m_dict.find(word) == m_dict.end()) {
            return std::vector<size_t>();
        }
        // get term id and docs
        size_t id = m_dict.at(word);
        const auto& doc_vec = m_index.at(id);

        // get document IDs containing this term
        std::vector<size_t> postings(doc_vec.size());
        std::transform(doc_vec.begin(), doc_vec.end(), postings.begin(),
                       [](const auto& doc_pair) { return doc_pair.first; });

        // store the document IDs
        posting_lists.push_back(postings);
    }

    // return early in trivial cases
    if (posting_lists.empty()) {
        return std::vector<size_t>();
    } else if (posting_lists.size() == 1) {
        return posting_lists[0];
    }

    // sort the doc_id lists to process the smallest vectors first
    std::sort(posting_lists.begin(), posting_lists.end(),
              [](const auto& first, const auto& second) {
                  return first.size() < second.size();
              });

    // intersect the first two doc id lists
    std::vector<size_t> result;
    intersect(posting_lists[0].begin(), posting_lists[0].end(),
              posting_lists[1].begin(), posting_lists[1].end(),
              std::back_inserter(result));

    // intersect the cumulative result with the next list
    std::vector<size_t> tmp;
    for (size_t i = 2; i < posting_lists.size(); ++i) {
        intersect(result.begin(), result.end(), posting_lists[i].begin(),
                  posting_lists[i].end(), std::back_inserter(tmp));

        result = tmp;
        tmp.clear();
    }

    return result;
}

std::vector<size_t>
ir::QueryProcessor::proximity_query(const std::vector<std::string>& words,
                                    const std::vector<size_t>& dists) const {
    // get the common docs first
    auto common_docs = this->conjunctive_query(words);

    std::vector<size_t> result;
    if (common_docs.empty()) {
        return result;
    }

    // check if each document contains the proximity query
    for (const size_t doc_id : common_docs) {
        if (contains_proximity_query(doc_id, words, dists)) {
            result.push_back(doc_id);
        }
    }

    return result;
}

bool ir::QueryProcessor::contains_proximity_query(
    size_t doc_id, const std::vector<std::string>& words,
    const std::vector<size_t>& dists) const {

    // store the position vector of each word in the current doc with id doc_id
    std::vector<std::vector<size_t>> word_pos;
    for (const auto& word : words) {
        size_t word_id = m_dict.at(word);
        for (const auto& doc_pair : m_index.at(word_id)) {
            if (doc_pair.first == doc_id) {
                word_pos.push_back(doc_pair.second);
                break;
            }
        }
    }

    // check if a proximity sequence exists starting at any of the first words.
    // if so, return true
    for (size_t pos : word_pos[0]) {
        if (proximity_seq_exists(pos, word_pos.begin() + 1, word_pos.end(),
                                 dists.begin(), dists.end())) {
            return true;
        }
    }
    return false;
}
