#include "query_processor.hpp"
#include "util.hpp"
#include <algorithm>

QueryProcessor::QueryProcessor() {}

QueryProcessor::QueryProcessor(const term_id_map& dict, const id_pos_map& index)
    : m_dict(dict), m_index(index) {}

const term_id_map& QueryProcessor::dict() const { return this->m_dict; }

void QueryProcessor::dict(const term_id_map& dict) { this->m_dict = dict; }

const id_pos_map& QueryProcessor::index() const { return this->m_index; }

void QueryProcessor::index(const id_pos_map& index) { this->m_index = index; }

std::vector<size_t>
QueryProcessor::conjunctive_query(const std::vector<std::string>& words) const {
    std::vector<std::vector<size_t>> posting_lists;
    for (const auto& word : words) {
        if (m_dict.find(word) == m_dict.end()) {
            return std::vector<size_t>();
        }
        size_t id = m_dict.at(word);
        const auto& doc_vec = m_index.at(id);

        std::vector<size_t> postings(doc_vec.size());
        std::transform(doc_vec.begin(), doc_vec.end(), postings.begin(),
                       [](const auto& doc_pair) { return doc_pair.first; });

        posting_lists.push_back(postings);
    }

    if (posting_lists.empty()) {
        return std::vector<size_t>();
    } else if (posting_lists.size() == 1) {
        return posting_lists[0];
    }

    std::sort(posting_lists.begin(), posting_lists.end(),
              [](const auto& first, const auto& second) {
                  return first.size() < second.size();
              });

    std::vector<size_t> result;
    intersect(posting_lists[0].begin(), posting_lists[0].end(),
              posting_lists[1].begin(), posting_lists[1].end(),
              std::back_inserter(result));

    std::vector<size_t> tmp;
    for (size_t i = 2; i < posting_lists.size(); ++i) {
        intersect(result.begin(), result.end(),
                  posting_lists[i].begin(), posting_lists[i].end(),
                  std::back_inserter(tmp));

        result = tmp;
        tmp.clear();
    }

    return result;
}

std::vector<size_t>
QueryProcessor::proximity_query(const std::vector<std::string>& words,
                                const std::vector<size_t>& dists) const {
    return {};
}
