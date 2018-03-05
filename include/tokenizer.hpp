#pragma once

#include <vector>
#include <string>
#include "defs.hpp"

std::vector<std::string> get_doc_terms(const raw_doc& doc);

std::string normalize(const std::string& token);

void normalize_all(std::vector<std::string>& token_vec);
