/*
 * Copyright 2018 Esref Ozdemir
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <string>
#include <unordered_map>

#include "defs.hpp"

namespace ir {

/**
 * @brief Header that denotes the beginning of a document in Reuters sgm files.
 */
const std::string DOC_HEADER = "<REUTERS";

/**
 * @brief Field inside ir::DOC_HEADER that denotes ID of the document.
 */
const std::string ID_FIELD = "NEWID=\"";

/**
 * @brief Tag inside document denoting the beginning of document text.
 */
const std::string TXT_BEG_TAG = "<TEXT";

/**
 * @brief Tag inside document denoting the end of document text.
 */
const std::string TXT_END_TAG = "</TEXT";

/**
 * @brief Tag inside document text denoting the beginning of document title.
 */
const std::string TITLE_BEG_TAG = "<TITLE>";

/**
 * @brief Tag inside document text denoting the end of document title.
 */
const std::string TITLE_END_TAG = "</TITLE>";

/**
 * @brief Tag inside document text denoting the beginning of document text body.
 */
const std::string BODY_BEG_TAG = "<BODY>";

/**
 * @brief Tag inside document text denoting the end of document text body.
 */
const std::string BODY_END_TAG = "</BODY>";

/**
 * @brief Parse a Reuters sgm file from the beginning of the given input stream
 * and return a mapping from document IDS to their raw content.
 *
 * @param ifs Input stream containing a Reuters sgm file.
 *
 * @return Mapping from document IDs obtained from ir::ID_FIELD to document text
 * which is a combination of title and body of the document.
 */
raw_doc_index parse_file(std::istream& ifs);
} // namespace ir
