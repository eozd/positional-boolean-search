#include <dirent.h>

#include "file_manager.hpp"
#include "util.hpp"
#include <algorithm>
#include <cassert>
#include <fstream>

std::vector<std::string> get_file_list() {
    DIR* dirp = opendir(DATASET_DIR.c_str());

    struct dirent* dp;
    std::string filename;
    std::vector<std::string> file_list;
    const std::string ext = ".sgm";
    while ((dp = readdir(dirp)) != nullptr) {
        filename = dp->d_name;
        if (filename.find(ext) == filename.size() - ext.size()) {
            file_list.push_back(DATASET_DIR + '/' + filename);
        }
    }

    std::sort(file_list.begin(), file_list.end());
    return file_list;
}

void create_dict_file(const pos_inv_index& inverted_index) {
    size_t index = 0;
    std::ofstream ofs(DICT_PATH, std::ios_base::trunc);
    for (const auto& pair : inverted_index) {
        const auto& term = pair.first;
        ofs << term << ' ' << index << '\n';
        ++index;
    }
    ofs << std::flush;
}

void create_index_file(const pos_inv_index& inverted_index) {
    size_t index = 0;
    std::ofstream ofs(INDEX_PATH, std::ios_base::trunc);
    for (const auto& term_pair : inverted_index) {
        const auto& doc_vec = term_pair.second;

        ofs << index << '\n' << POS_LIST_BEG_TAG << '\n';
        for (const auto& doc_pair : doc_vec) {
            size_t doc_id = doc_pair.first;
            const auto& pos_vec = doc_pair.second;
            ofs << '\t' << doc_id << " :";
            for (size_t pos : pos_vec) {
                ofs << ' ' << pos;
            }
            ofs << '\n';
        }
        ofs << POS_LIST_END_TAG << '\n';
        ++index;
    }
    ofs << std::flush;
}

term_id_map read_dict_file() {
    term_id_map result;
    std::string term;
    size_t id;
    std::ifstream ifs(DICT_PATH);
    if (!ifs) {
        throw std::runtime_error(DICT_PATH + " does not exist in the current folder!");
    }
    while (ifs >> term >> id) {
        result[term] = id;
    }
    return result;
}

id_pos_map read_index_file() {
    id_pos_map result;
    size_t term_id, doc_id, pos;
    std::string tag;
    std::string pos_line;
    std::ifstream ifs(INDEX_PATH);
    if (!ifs) {
        throw std::runtime_error(INDEX_PATH + " does not exist in the current folder!");
    }
    while (ifs >> term_id) {
        ifs.ignore(1);
        std::getline(ifs, tag);
        assert(tag == POS_LIST_BEG_TAG);

        std::getline(ifs, pos_line);
        while (pos_line != POS_LIST_END_TAG) {
            auto first_space = pos_line.find(' ');
            doc_id = std::stoul(pos_line.substr(1, first_space - 1));

            size_t num_beg = first_space + 3;
            std::string pos_str = pos_line.substr(num_beg);
            std::vector<std::string> pos_str_vec = split(pos_str, " ");

            std::vector<size_t> pos_vec(pos_str_vec.size());
            std::transform(
                pos_str_vec.begin(), pos_str_vec.end(), pos_vec.begin(),
                [](const std::string& pos_str) { return std::stoul(pos_str); });

            result[term_id].emplace_back(doc_id, pos_vec);

            std::getline(ifs, pos_line);
        }
    }

    return result;
}
