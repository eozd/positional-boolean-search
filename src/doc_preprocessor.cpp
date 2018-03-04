#include "doc_preprocessor.hpp"

static std::unordered_map<std::string, char>
html_special_str_to_ascii() {
    return std::unordered_map<std::string, char>({
        {"&#1;", ' '},
        {"&#2;", ' '},
        {"&#3;", ' '},
        {"&#5;", 5},
        {"&#22;", ' '},
        {"&#27;", ' '},
        {"&#30;", 30},
        {"&#31;", 31},
        {"&#127;", ' '},
        {"&amp;", '&'},
        {"&lt;", '<'},
        {"&gt;", '>'},
    });
};

void convert_html_special_chars(raw_doc& doc) {
    size_t index;
    for (const auto& sym_pair : html_special_str_to_ascii()) {
        const auto& html_sym = sym_pair.first;
        char ascii_char = sym_pair.second;
        while ((index = doc.find(html_sym)) != std::string::npos) {
            for (size_t offset = 0; offset < html_sym.size() - 1; ++offset) {
                doc[index + offset] = ' ';
            }
            doc[index + html_sym.size() - 1] = ascii_char;
        }
    }
}
