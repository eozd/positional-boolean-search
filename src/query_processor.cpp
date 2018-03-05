#include "query_processor.hpp"

QueryProcessor::QueryProcessor() {}

QueryProcessor::QueryProcessor(const term_id_map& dict, const id_pos_map& index)
    : m_dict(dict), m_index(index) {}

std::vector<size_t>
QueryProcessor::conjunctive_query(const std::vector<std::string>& words) const {
    return {};
}

std::vector<size_t>
QueryProcessor::proximity_query(const std::vector<std::string>& words,
                                const std::vector<size_t>& dists) const {
    return {};
}

const term_id_map& QueryProcessor::dict() const { return this->m_dict; }

void QueryProcessor::dict(const term_id_map& dict) { this->m_dict = dict; }

const id_pos_map& QueryProcessor::index() const { return this->m_index; }

void QueryProcessor::index(const id_pos_map& index) { this->m_index = index; }
