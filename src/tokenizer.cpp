#include "tokenizer.hpp"
#include "util.hpp"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <file_manager.hpp>
#include <fstream>
#include <iostream>

extern int stem(char* p, int i, int j);

std::vector<std::string> tokenize(std::string& str) {
    return split(str, " \t\n\r\v\f");
}

std::string remove_punctuation(const std::string& token) {
    std::string result(token);

    // remove certain puncts from anywhere in the word
    std::replace(result.begin(), result.end(), '\"', '\'');
    std::replace(result.begin(), result.end(), ',', '\'');
    std::replace(result.begin(), result.end(), '<', '\'');
    std::replace(result.begin(), result.end(), '>', '\'');
    result.erase(std::remove(result.begin(), result.end(), '\''), result.end());

    // remove any kind of punct from the start and end of the word
    if (result.empty()) {
        return result;
    }

    for (size_t i = 0; result[i] != '\'' && !isalnum(result[i]); ++i) {
        result[i] = '\'';
    }
    for (size_t i = result.size() - 1; result[i] != '\'' && !isalnum(result[i]);
         --i) {
        result[i] = '\'';
    }

    result.erase(std::remove(result.begin(), result.end(), '\''), result.end());
    return result;
}

bool is_stopword(const std::string& word) {
    static std::vector<std::string> stopwords;

    if (stopwords.empty()) {
        std::ifstream ifs(STOPWORD_PATH);
        std::string stopword;
        while (ifs >> stopword) {
            stopwords.push_back(stopword);
        }
        assert(!stopwords.empty());

        std::sort(stopwords.begin(), stopwords.end());
    }

    return std::binary_search(stopwords.begin(), stopwords.end(), word);
}

std::string normalize(const std::string& token) {
    // remove punctuation using heuristics
    std::string result = remove_punctuation(token);
    // convert string to lowercase
    std::transform(result.begin(), result.end(), result.begin(), tolower);
    // if string is a stopword, return empty string
    if (is_stopword(result)) {
        return "";
    }
    // stem the word
    auto word_end = static_cast<size_t>(stem(&result[0], 0, result.size() - 1));
    result = result.substr(0, word_end + 1);

    return result;
}

void normalize_all(std::vector<std::string>& token_vec) {
    std::transform(token_vec.begin(), token_vec.end(), token_vec.begin(),
                   normalize);
    token_vec.erase(std::remove(token_vec.begin(), token_vec.end(), ""),
                    token_vec.end());
}

std::vector<std::string> get_doc_terms(const raw_doc& doc) {
    std::vector<std::string> tokens;

    // tokenization
    {
        raw_doc doc_copy(doc);
        tokens = tokenize(doc_copy);
    }
    // normalization
    normalize_all(tokens);

    return tokens;
}
