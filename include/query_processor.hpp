#pragma once

#include "defs.hpp"
#include <string>
#include <vector>

class QueryProcessor {
  public:
    QueryProcessor();
    QueryProcessor(const term_id_map& dict, const id_pos_map& index);
    std::vector<size_t>
    conjunctive_query(const std::vector<std::string>& words) const;
    std::vector<size_t> proximity_query(const std::vector<std::string>& words,
                                        const std::vector<size_t>& dists) const;

    const term_id_map& dict() const;
    void dict(const term_id_map& dict);
    const id_pos_map& index() const;
    void index(const id_pos_map& index);

  private:
    term_id_map m_dict;
    id_pos_map m_index;
};
