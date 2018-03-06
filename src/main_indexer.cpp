#include <algorithm>
#include <fstream>
#include <iostream>
#include <tokenizer.hpp>

#include "doc_preprocessor.hpp"
#include "file_manager.hpp"
#include "parser.hpp"

/**
 * @brief Return an index from document IDs to raw document content constructed
 * from all the documents in the given file_list.
 *
 * @param file_list vector of document paths containing the individual
 * documents to be extracted using ir::parse_file.
 *
 * @return Mapping from document ID to raw document content.
 */
ir::raw_doc_index docs_from_files(const std::vector<std::string>& file_list) {
    ir::raw_doc_index all_docs;
    for (const auto& filepath : file_list) {
        std::ifstream ifs(filepath);
        // get all the docs in the current file
        auto docs_per_file = ir::parse_file(ifs);
        // store all the docs in the result variable
        all_docs.insert(docs_per_file.begin(), docs_per_file.end());
    }

    return all_docs;
};

/**
 * @brief Return an index from document IDs to vectors of normalized terms in
 * the corresponding documents.
 *
 * @param raw_docs Index from document IDs to raw document content.
 *
 * @return Mapping from document IDs to vectors of normalized terms.
 */
ir::doc_term_index terms_from_raw_docs(const ir::raw_doc_index& raw_docs) {
    ir::doc_term_index term_docs;
    for (const auto& pair : raw_docs) {
        const size_t id = pair.first;
        const auto& raw_doc = pair.second;
        // get all the normalized terms in the raw document content and store in
        // document id
        term_docs[id] = ir::get_doc_terms(raw_doc);
    }
    return term_docs;
}

/**
 * @brief Build the positional inverted index from a mapping from document ID
 * to vectors of normalized terms in the corresponding documents.
 *
 * This function builds the positional inverted index by iterating over all
 * the terms of a document and setting the document id of each of those terms to
 * the current document. Position of each term is assigned to its index in the
 * term vector of its corresponding document.
 *
 * After this procedure, document vector of each term in the resulting index
 * is sorted with respect to the document id.
 *
 * @param term_docs Mapping from document IDs to vectors of normalized terms.
 *
 * @return Positional inverted index.
 */
ir::pos_inv_index build_pos_inv_index(const ir::doc_term_index& term_docs) {
    ir::pos_inv_index result;

    for (const auto& pair : term_docs) {
        size_t doc_id = pair.first;
        const auto& term_vec = pair.second;

        for (size_t pos = 0; pos < term_vec.size(); ++pos) {
            const auto& term = term_vec[pos];
            auto& doc_to_pos = result[term];

            // find pos index of the corresponding document
            auto doc_of_term_it = std::find_if(
                doc_to_pos.begin(), doc_to_pos.end(),
                [doc_id](const std::pair<size_t, std::vector<size_t>>& elem) {
                    return elem.first == doc_id;
                });

            if (doc_of_term_it == doc_to_pos.end()) {
                // no pos list for the doc; create one
                doc_to_pos.emplace_back(doc_id, std::vector<size_t>());
                // add the pos to the pos list
                doc_to_pos.back().second.push_back(pos);
            } else {
                // pos list exists for current doc; add the pos
                doc_of_term_it->second.push_back(pos);
            }
        }
    }

    // sort doc list of each term with respect to doc_id
    for (auto& pair : result) {
        auto& doc_vec = pair.second;
        std::sort(doc_vec.begin(), doc_vec.end(),
                  [](const auto& left, const auto& right) {
                      return left.first < right.first;
                  });
    }

    return result;
}

/**
 * @brief Main routine to parse Reuters sgm files, build the positional inverted
 * index and write the dictionary to ir::DICT_PATH and the index to
 * ir::INDEX_PATH.
 *
 * @return 0 if successful.
 */
int main() {
    // parse the files and read the docs
    auto raw_docs = docs_from_files(ir::get_data_file_list());

    // handle special html character sequences
    for (auto& pair : raw_docs) {
        auto& doc = pair.second;
        ir::convert_html_special_chars(doc);
    }

    // tokenize and normalize the documents
    auto term_docs = terms_from_raw_docs(raw_docs);

    // build the positional inverted index
    auto inverted_index = build_pos_inv_index(term_docs);

    // save the positional inverted index as two files
    ir::write_dict_file(inverted_index);
    ir::write_index_file(inverted_index);

    return 0;
}