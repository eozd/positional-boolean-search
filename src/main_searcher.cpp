#include "file_manager.hpp"
#include "query_processor.hpp"
#include "tokenizer.hpp"
#include <algorithm>
#include <iostream>
#include <util.hpp>

/**
 * @brief Tokenize conjunctive query by finding the query keywords and returning
 * the normalized versions of the keywords using ir::normalize.
 *
 * This function extracts the keywords \f$w_i\f$ in a conjunctive query
 *
 * \f$
 * w_1 \mbox{ AND } w_2 \mbox{ AND } \dots \mbox{ AND } w_n
 * \f$
 *
 * and returns a vector of normalized terms \f$t_1, t_2, \dots, t_n\f$ where
 * each \f$t_i\f$ is the normalized version of \f$w_i\f$ using ir::normalize.
 *
 * @param query A conjunctive query.
 *
 * @return std::vector of keywords \f$t_1, t_2, \dots, t_n\f$.
 *
 * @throws std::runtime_error If the given query is not in the specified format.
 */
std::vector<std::string> tokenize_conjunctive_query(const std::string& query) {
    // discard the query identifier
    std::string query_search_part = query.substr(2);

    // split the query with respect to space
    std::vector<std::string> tokens = ir::split(query_search_part, " ");
    if (tokens.size() % 2 == 0) {
        throw std::runtime_error("Invalid conjunctive query");
    }

    std::vector<std::string> words;
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];

        // word AND word AND ...
        if (i % 2 == 0) {
            words.push_back(token);
        } else if (token != "AND") {
            throw std::runtime_error("Invalid conjunctive query");
        }
    }

    // normalize words
    ir::normalize_all(words);
    return words;
}

/**
 * @brief Tokenize phrase query by finding the query keywords and returning
 * the normalized versions of the keywords using ir::normalize.
 *
 * This function extracts the keywords \f$w_i\f$ in a phrase query
 *
 * \f$
 * w_1 w_2 \dots w_n
 * \f$
 *
 * and returns a vector of normalized terms \f$t_1, t_2, \dots, t_n\f$ where
 * each \f$t_i\f$ is the normalized version of \f$w_i\f$ using ir::normalize.
 *
 * @param query A phrase query.
 *
 * @return std::vector of normalized terms \f$t_1, t_2, \dots, t_n\f$.
 */
std::vector<std::string> tokenize_phrase_query(const std::string& query) {
    // discard the query identifier
    std::string query_search_part = query.substr(2);

    // simply split the query with respect to space
    std::vector<std::string> words = ir::split(query_search_part, " ");

    // normalize words
    ir::normalize_all(words);
    return words;
}

/**
 * @brief Tokenize proximity query by finding the query keywords and distances,
 * and returning a pair of normalized term vector and distance vector.
 *
 * This function extracts the keywords \f$w_i\f$ and distances \f$k_i\f$ in a
 * proximity query
 *
 * \f$
 * w_1 \mbox{  } /k_1 \mbox{  } w_2 \mbox{  } /k_2 \mbox{  } \dots \mbox{  }
 * /k_{n-1} \mbox{  } w_n \f$
 *
 * and returns a pair of normalized term vector \f$t_1, t_2, \dots, t_n\f$ and
 * distance vector \f$k_1, k_2, \dots, k_{n-1}\f$.
 *
 * @param query A proximity query.
 *
 * @return pair of normalized term vector \f$t_1, t_2, \dots, t_n\f$ and
 * distance vector \f$k_1, k_2, \dots, k_{n-1}\f$.
 *
 * @throws std::runtime_error If the given query is not in the specified format.
 */
std::pair<std::vector<std::string>, std::vector<size_t>>
tokenize_proximity_query(const std::string& query) {
    // discard the query identifier
    std::string query_search_part = query.substr(2);

    // split the query with respect to space
    std::vector<std::string> tokens = ir::split(query_search_part, " ");
    if (tokens.size() % 2 == 0) {
        throw std::runtime_error("Invalid proximity query");
    }

    std::vector<std::string> words;
    std::vector<size_t> dists;
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];

        // word /k_1 word /k_2 ...
        if (i % 2 == 0) {
            words.push_back(token);
        } else {
            // must start with /
            // must be at least two characters
            // all the chars after / must be digits
            if (!(token[0] == '/' && token.size() > 1 &&
                  std::all_of(token.begin() + 1, token.end(), isdigit))) {
                throw std::runtime_error("Invalid proximity query");
            }
            dists.push_back(std::stoul(token.substr(1)));
        }
    }

    // normalize words
    ir::normalize_all(words);

    return {words, dists};
}

/**
 * @brief Main routine to read the indices constructed using indexer and answer
 * conjunctive, phrase and proximity queries in an infinite input loop.
 *
 * User input is taken from STDIN and output is written to STDOUT.
 *
 * @return 0 if the program is terminated using Ctrl-D, -1 if there is a problem
 * in reading index files.
 */
int main() {
    std::cout << "Reading index files..." << std::flush;
    ir::QueryProcessor query_processor;
    try {
        query_processor =
            ir::QueryProcessor(ir::read_dict_file(), ir::read_index_file());
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
    std::cout << "OK!\n" << std::endl;

    // query headers: 1 --> conjunctive, 2 --> phrase, 3 --> proximity
    const std::string query_headers = "123";

    std::string query;
    while (std::cin) {
        std::cout << "Please enter a search query and press Enter\n> "
                  << std::flush;
        std::getline(std::cin, query);
        if (std::cin.eof()) {
            return 0;
        }

        // query must be at least length 3
        // first char must be space
        // second char must start a word
        // zeroth char must be one of 1 2 3
        bool proper_query =
            query.size() > 2 && query[1] == ' ' && !isspace(query[2]) &&
            ir::one_of(query_headers.begin(), query_headers.end(), query[0]);

        if (!proper_query) {
            std::cout
                << "Your query must be in the format: <query_type> "
                   "<query>\nwhere\n"
                   "\tquery_type == 1 --> conjunctive query:\t<w1> AND <w2> "
                   "AND ... AND <wn>\n"
                   "\tquery_type == 2 --> phrase query:\t<w1> <w2> ... <wn>\n"
                   "\tquery_type == 3 --> proximity query:\t<w1> /k1 <w2> /k2 "
                   "... /kn <wn+1>\n"
                << std::endl;
            continue;
        }

        std::vector<size_t> results;
        try {
            if (query[0] == '1') {
                // get the query tokens
                auto tokens = tokenize_conjunctive_query(query);
                // search
                results = query_processor.conjunctive_query(tokens);
            } else if (query[0] == '2') {
                // get the query tokens
                auto tokens = tokenize_phrase_query(query);
                // phrase query is proximity query with all distances equal to 0
                std::vector<size_t> dists(tokens.size() - 1, 0);

                // search
                results = query_processor.proximity_query(tokens, dists);
            } else if (query[0] == '3') {
                // get the query tokens and distances
                std::vector<std::string> tokens;
                std::vector<size_t> dists;
                std::tie(tokens, dists) = tokenize_proximity_query(query);

                // search
                results = query_processor.proximity_query(tokens, dists);
            }
        } catch (const std::runtime_error& e) {
            std::cout << e.what() << std::endl;
            continue;
        }
        // sort the document ids
        std::sort(results.begin(), results.end());

        if (results.empty()) {
            std::cout << "No match found!" << std::endl;
        } else {
            std::cout << "Matching documents:" << std::endl;
            for (size_t doc_id : results) {
                std::cout << doc_id << '\n';
            }
            std::cout << std::flush;
        }
    }
}
