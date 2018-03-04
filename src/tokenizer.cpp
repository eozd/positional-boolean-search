#include "tokenizer.hpp"


extern int stem(char * p, int i, int j);

std::vector<std::string> get_doc_terms(const raw_doc& doc) {
    // todo: split by space
    // todo: handle punctuation somehow
    // todo: case-folding
    // todo: stopword removal
    // todo: stemming
    //std::string test = "computer";
    //auto last = static_cast<size_t>(stem(&test[0], 0, test.size() - 1));
    //std::vector<std::string> res;
    //res.push_back(test.substr(0, last + 1));
    //return res;
}
