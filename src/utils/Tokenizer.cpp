#include "Tokenizer.h"
#include <cctype>

std::vector<std::string> Tokenizer::tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    std::string cur;

    for (char ch : text) {
        if (std::isalnum(static_cast<unsigned char>(ch))) {
            cur.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
        } else {
            if (!cur.empty()) {
                tokens.push_back(cur);
                cur.clear();
            }
        }
    }

    if (!cur.empty()) tokens.push_back(cur);
    return tokens;
}
