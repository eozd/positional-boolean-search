#include <dirent.h>

#include <algorithm>
#include "file_manager.hpp"

std::vector<std::string> get_file_list() {
    DIR* dirp = opendir(DATASET_DIR.c_str());

    struct dirent* dp;
    std::string filename;
    std::vector<std::string> file_list;
    while ((dp = readdir(dirp)) != nullptr) {
        filename = dp->d_name;
        if (filename.find(".sgm") != std::string::npos) {
            file_list.push_back(DATASET_DIR + '/' + filename);
        }
    }

    std::sort(file_list.begin(), file_list.end());
    return file_list;
}
