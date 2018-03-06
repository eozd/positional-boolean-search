#include "query_processor.hpp"
#include "util.hpp"
#include <algorithm>

//ir::QueryProcessor::QueryProcessor() = default;

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
    return {};
}
