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

template <typename InputIterator1, typename InputIterator2>
bool recurse(size_t pos, InputIterator1 pos_begin, InputIterator1 pos_end,
             InputIterator2 dist_begin, InputIterator2 dist_end) {
    if (pos_begin == pos_end) {
        assert(dist_begin == dist_end);
        return false;
    }
    size_t dist = *dist_begin;
    size_t max_pos = pos + dist + 1;
    const auto& pos_vec = *pos_begin;

    auto first_greater_it =
        std::upper_bound(pos_vec.begin(), pos_vec.end(), pos);

    if (first_greater_it == pos_vec.end() || *first_greater_it > max_pos) {
        return false;
    } else if (pos_begin + 1 == pos_end) {
        assert(dist_begin + 1 == dist_end);
        return true;
    }

    std::vector<size_t> valid_next_pos;
    for (auto it = first_greater_it; it != pos_vec.end() && *it <= max_pos;
         ++it) {
        valid_next_pos.push_back(*it);
    }

    for (size_t next_pos : valid_next_pos) {
        if (recurse(next_pos, pos_begin + 1, pos_end, dist_begin + 1,
                    dist_end)) {
            return true;
        }
    }
    return false;
};

bool ir::QueryProcessor::contains_proximity_query(
    size_t doc_id, const std::vector<std::string>& words,
    const std::vector<size_t>& dists) const {

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

    for (size_t pos : word_pos[0]) {
        if (recurse(pos, word_pos.begin() + 1, word_pos.end(), dists.begin(),
                    dists.end())) {
            return true;
        }
    }
    return false;
}

std::vector<size_t>
ir::QueryProcessor::proximity_query(const std::vector<std::string>& words,
                                    const std::vector<size_t>& dists) const {
    auto common_docs = this->conjunctive_query(words);

    std::vector<size_t> result;
    if (common_docs.empty()) {
        return result;
    }

    for (const size_t doc_id : common_docs) {
        if (contains_proximity_query(doc_id, words, dists)) {
            result.push_back(doc_id);
        }
    }

    return result;
}
