#include "tokenizer.hpp"
#include "util.hpp"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <file_manager.hpp>
#include <fstream>
#include <iostream>

// tell the compiler that stem will be externally linked
extern int stem(char* p, int i, int j);

/**
 * @brief Split the given string with respect to whitespace characters and
 * return the resulting tokens as a vector.
 *
 * @param str Input string to tokenize.
 * @return
 */
std::vector<std::string> tokenize(const std::string& str) {
    std::string str_copy(str);
    return split(str_copy, " \t\n\r\v\f");
}

/**
 * @brief Remove certain punctuation characters from certain parts of the given
 * string and return a copy.
 *
 * Punctuation characters are removed as specified in ir::normalize.
 *
 * @param token Input token from which punctuation will be removed.
 *
 * @return String with certain punctuation characters removed from token.
 */
std::string remove_punctuation(const std::string& token) {
    std::string result(token);

    // remove certain puncts from anywhere in the word
    std::replace(result.begin(), result.end(), '\"', '\'');
    std::replace(result.begin(), result.end(), ',', '\'');
    std::replace(result.begin(), result.end(), '<', '\'');
    std::replace(result.begin(), result.end(), '>', '\'');
    result.erase(std::remove(result.begin(), result.end(), '\''), result.end());

    if (result.empty()) {
        return result;
    }

    // iterate as long as current char is not alphanumeric and not '
    // (' will be removed)
    auto to_remove = [](const char c) { return c != '\'' && !isalnum(c); };
    // remove any kind of punct from the start and end of the word
    for (size_t i = 0; to_remove(result[i]); ++i) {
        result[i] = '\'';
    }
    for (size_t i = result.size() - 1; to_remove(result[i]); --i) {
        result[i] = '\'';
    }
    result.erase(std::remove(result.begin(), result.end(), '\''), result.end());

    return result;
}

/**
 * @brief Check whether the input string is a stopword.
 *
 * This function simply checks if the given word is in the stopword list
 * defined in ir::STOPWORD_PATH.
 *
 * For efficiency purposes, the file is read only once when the function is
 * called for the first time, and the stopword list is sorted. Then, stopword
 * check is done using binary search.
 *
 * @param word Word to check if it is a stopword.
 *
 * @return true if word is in stopword list; false, otherwise.
 */
bool is_stopword(const std::string& word) {
    static std::vector<std::string> stopwords;

    // if calling for the first time
    if (stopwords.empty()) {
        std::ifstream ifs(ir::STOPWORD_PATH);
        std::string stopword;
        while (ifs >> stopword) {
            stopwords.push_back(stopword);
        }
        assert(!stopwords.empty());

        std::sort(stopwords.begin(), stopwords.end());
    }

    return std::binary_search(stopwords.begin(), stopwords.end(), word);
}

std::string ir::normalize(const std::string& token) {
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

void ir::normalize_all(std::vector<std::string>& token_vec) {
    // normalize all words in-place
    std::transform(token_vec.begin(), token_vec.end(), token_vec.begin(),
                   normalize);
    // remove empty strings
    token_vec.erase(std::remove(token_vec.begin(), token_vec.end(), ""),
                    token_vec.end());
}

std::vector<std::string> ir::get_doc_terms(const raw_doc& doc) {
    auto tokens = tokenize(doc);
    normalize_all(tokens);

    return tokens;
}
