#pragma once

#include "defs.hpp"
#include <string>
#include <vector>

const std::string DATASET_DIR = "Dataset";
const std::string STOPWORD_PATH = "stopwords.txt";
const std::string DICT_PATH = "dict.txt";
const std::string INDEX_PATH = "index.txt";
const std::string POS_LIST_BEG_TAG = "<POS_LIST>";
const std::string POS_LIST_END_TAG = "</POS_LIST>";

std::vector<std::string> get_file_list();
void create_dict_file(const pos_inv_index& inverted_index);
void create_index_file(const pos_inv_index& inverted_index);
term_id_map read_dict_file();
id_pos_map read_index_file();
