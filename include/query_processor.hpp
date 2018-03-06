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

#pragma once

#include "defs.hpp"
#include <algorithm>
#include <cassert>
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
    QueryProcessor() = default;

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
    /**
     * @brief Check if the document with the given id contains a proximity query
     * specified by words and dists vectors.
     *
     * This function starts a recursive positional search from every occurrence
     * of the first word given in words vector. If any of the sequences that
     * obey the given distances exist in the document, the function retuns true.
     *
     * @param doc_id ID of the document to check if it contains the given
     * proximity query.
     * @param words std::vector of words to search as specified in
     * QueryProcessor::proximity_query.
     * @param dists std::vector of distances as specified in
     * QueryProcessor::proximity_query.
     *
     * @return true if the given proximity query exists in the document with the
     * given ID; false, otherwise.
     */
    bool contains_proximity_query(size_t doc_id,
                                  const std::vector<std::string>& words,
                                  const std::vector<size_t>& dists) const;

  private:
    term_id_map m_dict;
    id_pos_map m_index;
};

/**
 * @brief Check if a position sequence as specified in
 * QueryProcessor::proximity_query exists in the given position and distance
 * sequences.
 *
 * This function checks if a sequence of positions that are certain distance
 * apart from each other exist in the given position and distance sequences.
 *
 * Each entry of position iterators (pos_begin and pos_end) must contain an
 * iterable that in turn contains the positions of that term in the document.
 *
 * Each entry of distance iterators (dist_begin and dist_end) must contain the
 * maximum allowable distance between the current position as given in pos and
 * the position of the next term.
 *
 * This function recursively searches all possible branches to construct a
 * sequence of positions.
 *
 * @tparam VectorIterator Iterator whose dereferenced entry corresponds to a
 * vector of positions.
 * @tparam IntIterator Iterator whose dereferenced entry corresponds to a
 * distance.
 * @param pos Position of the current term.
 * @param pos_begin Iterator pointing to the positions of the next term.
 * @param pos_end Iterator pointing to the end of the position vector sequence.
 * @param dist_begin Iterator pointing to the maximum allowable distance between
 * the current term (whose position is given by pos) and the next term.
 * @param dist_end Iterator pointing to the end of the distance sequence.
 *
 * @return true if a proximity sequence is constructable from the given
 * positions and distances; false, otherwise.
 */
template <typename VectorIterator, typename IntIterator>
bool proximity_seq_exists(size_t pos, VectorIterator pos_begin,
                          VectorIterator pos_end, IntIterator dist_begin,
                          IntIterator dist_end) {
    // if no value was returned at this point, seq doesn't exist
    if (pos_begin == pos_end) {
        assert(dist_begin == dist_end);
        return false;
    }
    size_t curr_dist = *dist_begin;
    // max allowed position index
    size_t max_pos = pos + curr_dist + 1;
    const auto& pos_vec = *pos_begin;

    // find the smallest value greater than current position via binary search
    auto first_greater_it =
        std::upper_bound(pos_vec.begin(), pos_vec.end(), pos);

    if (first_greater_it == pos_vec.end() || *first_greater_it > max_pos) {
        // if no such value exists or it is too far
        return false;
    } else if (pos_begin + 1 == pos_end) {
        // if we are at the end of the search list, return true; otherwise, we
        // should check the rest of the sequence
        assert(dist_begin + 1 == dist_end);
        return true;
    }

    // get all the next positions that are <= max_pos
    std::vector<size_t> valid_next_pos;
    for (auto it = first_greater_it; it != pos_vec.end() && *it <= max_pos;
         ++it) {
        valid_next_pos.push_back(*it);
    }

    // check if a sequence from any of them exists; if so, return true.
    for (size_t next_pos : valid_next_pos) {
        if (proximity_seq_exists(next_pos, pos_begin + 1, pos_end,
                                 dist_begin + 1, dist_end)) {
            return true;
        }
    }
    // no sequence was found.
    return false;
};
} // namespace ir
