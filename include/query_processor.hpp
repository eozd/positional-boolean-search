#pragma once

#include "defs.hpp"
#include <string>
#include <vector>

namespace ir {

/**
 * @brief QueryProcessor is a class to store the positional inverted index and
 * return the results to search queries.
 *
 * QueryProcessor defines its own interface that specifies what type of
 * parameters to pass for each query. Hence, QueryProcessor is not responsible
 * for raw query parsing and processing; its responsibility is to compute the
 * results of queries.
 */
class QueryProcessor {
  public:
    /**
     * @brief Default constructor that constructs a QueryProcessor with empty
     * positional inverted index.
     *
     * A QueryProcessor constructed using the default constructor cannot give
     * an answer to any query. This constructor is merely for convenience
     * purposes related to scoping.
     */
    QueryProcessor();

    /**
     * @brief Positional inverted index constructor that constructs a
     * QueryProcessor by copying the index.
     *
     * @param dict Dictionary from terms to their unique IDs.
     * @param index Index from term IDs to positional posting lists.
     */
    QueryProcessor(const term_id_map& dict, const id_pos_map& index);

    /**
     * @brief Compute the result of a conjunctive query.
     *
     * A conjunctive query is of the form
     *
     * \f$
     * w_1 \mbox{ AND } w_2 \mbox{ AND } \dots \mbox{ AND } w_n
     * \f$.
     *
     * The resulting documents of a conjunctive query contains all the specified
     * words.
     *
     * @param words std::vector of words \f$w_1, w_2, \dots, w_n\f$.
     *
     * @return std::vector of document IDs containing all the words in the
     * query.
     */
    std::vector<size_t>
    conjunctive_query(const std::vector<std::string>& words) const;

    /**
     * @brief Compute the result of a proximity query.
     *
     * A proximity query is of the form
     *
     * \f$
     * w_1 \mbox{  } /k_1 \mbox{  } w_2 \mbox{  } /k_2 \mbox{  } \dots \mbox{  }
     * /k_{n-1} \mbox{  } w_n \f$
     *
     * where positional difference between words \f$w_i\f$ and \f$w_{i+1}\f$ can
     * be at most \f$k_i\f$ where \f$k_i\f$ is a nonnegative integer.
     *
     * @param words std::vector of words \f$w_1, w_2, \dots, w_n\f$.
     * @param dists std::vector of distances \f$k_1, k_2, \dots k_{n-1}\f$.
     *
     * @return std::vector of document IDs containing a sequence of words that
     * matches the given proximity query.
     */
    std::vector<size_t> proximity_query(const std::vector<std::string>& words,
                                        const std::vector<size_t>& dists) const;

  private:
    term_id_map m_dict;
    id_pos_map m_index;
};
} // namespace ir
