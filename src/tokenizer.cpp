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

std::vector<std::string> get_doc_terms(const raw_doc& doc) {
    std::vector<std::string> tokens;

    // tokenization
    {
        raw_doc doc_copy(doc);
        tokens = tokenize(doc_copy);
    }

    // remove punctuation
    for (auto& token : tokens) {
        token = remove_punctuation(token);
    }

    // case-folding
    for (auto& token : tokens) {
        std::transform(token.begin(), token.end(), token.begin(), tolower);
    }

    // stopword removal
    for (auto& token : tokens) {
        if (is_stopword(token)) {
            token.clear();
        }
    }
    tokens.erase(std::remove(tokens.begin(), tokens.end(), ""), tokens.end());

    // stemming
    for (auto& token : tokens) {
        auto word_end =
            static_cast<size_t>(stem(&token[0], 0, token.size() - 1));
        token = token.substr(0, word_end + 1);
    }

    return tokens;
}
