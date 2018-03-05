#include "file_manager.hpp"
#include "query_processor.hpp"
#include "tokenizer.hpp"
#include <algorithm>
#include <iostream>
#include <util.hpp>

std::vector<std::string> tokenize_conjunctive_query(const std::string& query) {
    std::string query_search_part = query.substr(2);
    std::vector<std::string> tokens = split(query_search_part, " ");
    if (tokens.size() % 2 == 0) {
        throw std::runtime_error("Invalid conjunctive query");
    }

    std::vector<std::string> words;
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];

        if (i % 2 == 0) {
            words.push_back(token);
        } else if (token != "AND") {
            throw std::runtime_error("Invalid conjunctive query");
        }
    }

    // normalize words
    normalize_all(words);
    return words;
}

std::vector<std::string> tokenize_phrase_query(const std::string& query) {
    std::string query_search_part = query.substr(2);
    std::vector<std::string> words = split(query_search_part, " ");

    // normalize words
    normalize_all(words);
    return words;
}

std::pair<std::vector<std::string>, std::vector<size_t>>
tokenize_proximity_query(const std::string& query) {
    std::string query_search_part = query.substr(2);
    std::vector<std::string> tokens = split(query_search_part, " ");
    if (tokens.size() % 2 == 0) {
        throw std::runtime_error("Invalid proximity query");
    }

    std::vector<std::string> words;
    std::vector<size_t> dists;
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        if (i % 2 == 0) {
            words.push_back(token);
        } else {
            if (!(token[0] == '/' && token.size() > 1 &&
                  std::all_of(token.begin() + 1, token.end(), isdigit))) {
                throw std::runtime_error("Invalid proximity query");
            }
            dists.push_back(std::stoul(token.substr(1)));
        }
    }

    // normalize words
    normalize_all(words);

    return {words, dists};
}

int main() {
    QueryProcessor query_processor;
    try {
        auto dict = read_dict_file();
        auto index = read_index_file();

        query_processor.dict(dict);
        query_processor.index(index);
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }

    const std::string query_headers = "012";

    std::string query;
    while (std::cin) {
        std::cout << "Please enter a search query and press Enter\n> "
                  << std::flush;
        std::getline(std::cin, query);
        if (std::cin.eof()) {
            return 0;
        }

        bool proper_query =
            query.size() > 2 && query[1] == ' ' && !isspace(query[2]) &&
            one_of(query_headers.begin(), query_headers.end(), query[0]);
        if (!proper_query) {
            std::cout
                << "Your query must be in the format: <query_type> "
                   "<query>\nwhere\n"
                   "\tquery_type == 0 --> conjunctive query:\t<w1> AND <w2> "
                   "AND ... AND <wn>\n"
                   "\tquery_type == 1 --> phrase query:\t<w1> <w2> ... <wn>\n"
                   "\tquery_type == 2 --> proximity query:\t<w1> /k1 <w2> /k2 "
                   "... /kn <wn+1>\n"
                << std::endl;
            continue;
        }

        std::vector<size_t> results;
        try {
            if (query[0] == '0') {
                auto tokens = tokenize_conjunctive_query(query);
                results = query_processor.conjunctive_query(tokens);
            } else if (query[0] == '1') {
                auto tokens = tokenize_phrase_query(query);
                std::vector<size_t> dists(tokens.size() - 1, 0);

                results = query_processor.proximity_query(tokens, dists);
            } else if (query[0] == '2') {
                std::vector<std::string> tokens;
                std::vector<size_t> dists;
                std::tie(tokens, dists) = tokenize_proximity_query(query);

                results = query_processor.proximity_query(tokens, dists);
            }
        } catch (const std::runtime_error& e) {
            std::cout << e.what() << std::endl;
            continue;
        }
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
