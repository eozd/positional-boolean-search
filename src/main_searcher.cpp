#include "file_manager.hpp"
#include "query_processor.hpp"
#include <algorithm>
#include <iostream>
#include <util.hpp>

int main() {
    try {
        auto dict = read_dict_file();
        auto index = read_index_file();
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
            std::cout << "Your query must be in the format: <query_type> "
                         "<query>\nwhere\n"
                         "\tquery_type == 0 --> conjunctive query:\t<w1> AND <w2> AND ... AND <wn>\n"
                         "\tquery_type == 1 --> phrase query:\t<w1> <w2> ... <wn>\n"
                         "\tquery_type == 2 --> proximity query:\t<w1> /k1 <w2> /k2 ... /kn <wn+1>\n"
                      << std::endl;
            continue;
        }

        std::vector<size_t> results;
        if (query[0] == '0') {
            results = conjunctive_query(query);
        } else if (query[0] == '1') {
            results = phrase_query(query);
        } else if (query[0] == '2') {
            results = proximity_query(query);
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
