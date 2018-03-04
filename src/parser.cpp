#include "parser.hpp"

#include <cassert>
#include <iostream>

int get_doc_id(const std::string& header_line) {
    size_t newid_pos = header_line.find(ID_FIELD);
    size_t num_beg_pos = newid_pos + ID_FIELD.size();
    size_t num_end_pos = header_line.find('\"', num_beg_pos);

    std::string num_str =
            header_line.substr(num_beg_pos, num_end_pos - num_beg_pos);

    return std::atoi(num_str.c_str());
}

raw_doc get_next_doc(std::istream& ifs) {
    std::string line;
    size_t beg_header_pos;
    size_t end_header_pos;
    while (std::getline(ifs, line)) {
        beg_header_pos = line.find(TXT_BEG_TAG);
        if (beg_header_pos != std::string::npos) {
            std::string text;

            do {
                std::getline(ifs, line);
                end_header_pos = line.find(TXT_END_TAG);
                text += "\n" + line.substr(0, end_header_pos);
            } while (end_header_pos == std::string::npos);

            return text;
        }
    }
    throw std::invalid_argument("Input stream does not contain " + TXT_BEG_TAG +
                                " and " + TXT_END_TAG + " fields");
}

std::string text_between_tags(const std::string& doc_text,
                              const std::string& beg_tag,
                              const std::string& end_tag) {
    size_t tag_beg_pos = doc_text.find(beg_tag);
    if (tag_beg_pos == std::string::npos) {
        return "";
    }

    size_t tag_end_pos = doc_text.find(end_tag);
    assert(tag_end_pos != std::string::npos);

    size_t beg_index = tag_beg_pos + beg_tag.size();
    size_t len = tag_end_pos - beg_index;

    return doc_text.substr(beg_index, len);
}

raw_doc_index parse_file(std::istream& ifs) {
    raw_doc_index docs;
    std::string line;

    int id;
    raw_doc doc;
    while (std::getline(ifs, line)) {
        if (line.find(DOC_HEADER) == 0) {
            id = get_doc_id(line);
            doc = get_next_doc(ifs);
            doc =
                    text_between_tags(doc, TITLE_BEG_TAG, TITLE_END_TAG) +
                    "\n" + text_between_tags(doc, BODY_BEG_TAG, BODY_END_TAG);
            docs[id] = doc;
        }
    }

    return docs;
};

