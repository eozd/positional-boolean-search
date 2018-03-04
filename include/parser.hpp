#pragma once

#include <unordered_map>
#include <string>

#include "defs.hpp"

const std::string DOC_HEADER = "<REUTERS";
const std::string ID_FIELD = "NEWID=\"";
const std::string TXT_BEG_TAG = "<TEXT";
const std::string TXT_END_TAG = "</TEXT";
const std::string TITLE_BEG_TAG = "<TITLE>";
const std::string TITLE_END_TAG = "</TITLE>";
const std::string BODY_BEG_TAG = "<BODY>";
const std::string BODY_END_TAG = "</BODY>";


raw_doc_index parse_file(std::istream& ifs);
