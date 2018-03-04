#include <algorithm>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <tokenizer.hpp>

#include "file_manager.hpp"
#include "parser.hpp"
#include "doc_preprocessor.hpp"


raw_doc_index
docs_from_files(const std::vector<std::string>& file_list) {
    raw_doc_index all_docs;
    for (const auto& filepath : file_list) {
        std::ifstream ifs(filepath);
        auto docs_per_file = parse_file(ifs);
        all_docs.insert(docs_per_file.begin(), docs_per_file.end());
    }

    return all_docs;
};

doc_term_index terms_from_raw_docs(const raw_doc_index& raw_docs) {
    doc_term_index term_docs;
    for (const auto& pair : raw_docs) {
        const size_t id = pair.first;
        const auto& raw_doc = pair.second;
        term_docs[id] = get_doc_terms(raw_doc);
        continue;
    }
    return term_docs;
}

int main() {
    auto raw_docs = docs_from_files(get_file_list());

    for (auto& pair : raw_docs) {
        auto& doc = pair.second;
        convert_html_special_chars(doc);
    }

    auto term_docs = terms_from_raw_docs(raw_docs);

    // todo: build the positional inverted positional index

    // todo: write terms as a map<term, id>
    // todo: write positional inverted index as a map<id, postings_list>

    return 0;
}