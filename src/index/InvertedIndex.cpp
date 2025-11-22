#include "InvertedIndex.h"
#include "../utils/FileLoader.h"
#include "../utils/Tokenizer.h"
#include <chrono>
#include <iostream>

void InvertedIndex::addDocument(int docId, const std::string& path) {
    documents_[docId] = path;
}

void InvertedIndex::build() {
    index_.clear();

    auto start = std::chrono::high_resolution_clock::now();

    for (const auto& [docId, path] : documents_) {
        std::string text = FileLoader::readAll(path);
        auto words = Tokenizer::tokenize(text);

        for (const auto& w : words) {
            index_[w].insert(docId);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start).count();

    std::cout << "[Sequential Index] Build time: "
              << duration << " seconds.\n";
}


std::vector<int> InvertedIndex::search(const std::string& word) const {
    std::string key = word;
    for (auto& c : key) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

    auto it = index_.find(key);
    if (it == index_.end()) return {};

    return std::vector<int>(it->second.begin(), it->second.end());
}

const std::string& InvertedIndex::docPath(int docId) const {
    return documents_.at(docId);
}
