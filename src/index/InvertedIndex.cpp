#include "InvertedIndex.h"
#include "../utils/FileLoader.h"
#include "../utils/Tokenizer.h"
#include "../threading/ThreadPool.h"

#include <chrono>
#include <iostream>
#include <cctype>
#include <future>

void InvertedIndex::addDocument(int docId, const std::string& path) {
    documents_[docId] = path;
}

void InvertedIndex::build(int numThreads) {
    index_.clear();

    auto start = std::chrono::high_resolution_clock::now();

    if (numThreads <= 1) {
        for (const auto& [docId, path] : documents_) {
            std::string text = FileLoader::readAll(path);
            auto words = Tokenizer::tokenize(text);

            for (const auto& w : words) {
                index_.add(w, docId);
            }
        }
    } else {
        ThreadPool pool(numThreads);
        std::vector<std::future<void>> futures;
        futures.reserve(documents_.size());

        for (const auto& [docId, path] : documents_) {
            futures.emplace_back(pool.enqueue([this, docId, path]() {
                std::string text = FileLoader::readAll(path);
                auto words = Tokenizer::tokenize(text);

                for (const auto& w : words) {
                    index_.add(w, docId);
                }
            }));
        }

        for (auto& f : futures) f.get();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start).count();

    std::cout << "[Index N=" << numThreads << "] Build time: "
              << duration << " seconds.\n";
}

std::vector<int> InvertedIndex::search(const std::string& word) const {
    std::string key = word;
    for (auto& c : key) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return index_.get(key);
}

const std::string& InvertedIndex::docPath(int docId) const {
    return documents_.at(docId);
}
