#pragma once

#include "defs.hpp"
#include <string>
#include <vector>

namespace ir {
/**
 * @brief Tokenize and normalize a given raw document and return a vector of
 * terms.
 *
 * This function tokenizes the given raw document by splitting it by whitespace,
 * and then does normalization operations to each token as defined in
 * ir::normalize.
 *
 * @param doc Raw document.
 *
 * @return std::vector of normalized terms in the given raw document.
 */
std::vector<std::pair<std::string, size_t>> get_doc_terms(const raw_doc& doc);

/**
 * @brief Return the normalized version a given token.
 *
 * Token normalization consists of the following steps:
 *
 * 1. Punctuation removal
 *   * All punctuation characters at the beginning and at the end of the token
 * is removed. Additionally, <blockquote>' " < > </blockquote> characters are
 * removed from anywhere in the token.
 *
 * 2. Case folding
 *   * All characters are converted to lowercase.
 * 3. Stopword removal
 *   * If the given token is a stopword, an empty string is returned.
 * 4. Stemming
 *   * The resulting token is stemmed using the Porter Stemmer.
 *
 * @param token Token to normalize.
 *
 * @return Normalized version of the token. IF the given token is a stopword,
 * an empty string is returned.
 */
std::string normalize(const std::string& token);

/**
 * @brief Normalize all the tokens in the given vector of tokens in-place.
 *
 * This function is a convenience function to apply ir::normalize to each
 * element of input token_vec and then remove empty strings from the vector.
 *
 * @param token_vec vector of tokens to normalize in-place.
 */
void normalize_all(std::vector<std::string>& token_vec);
} // namespace ir
