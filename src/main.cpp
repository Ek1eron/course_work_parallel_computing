#include <iostream>
#include <filesystem>
#include "index/InvertedIndex.h"

namespace fs = std::filesystem;

int main() {
    InvertedIndex idx;

    std::string dataRoot = "data";

    int docId = 0;
    int progressCounter = 0;

    for (auto& p : fs::recursive_directory_iterator(dataRoot)) {
        if (p.is_regular_file() && p.path().extension() == ".txt") {
            idx.addDocument(docId++, p.path().string());
            progressCounter++;

            if (progressCounter % 100 == 0) {
                std::cout << "[Loading] Processed " << progressCounter << " files...\n";
            }
        }
    }

    std::cout << "Loaded " << docId << " documents.\n";

    int N = 8;
    idx.build(N);
    std::cout << "Index built!\n";

    while (true) {
        std::string word;
        std::cout << "Enter word to search (or 'exit'): ";
        std::cin >> word;
        if (word == "exit") break;

        auto docs = idx.search(word);
        std::cout << "Found in " << docs.size() << " documents:\n";
        for (int id : docs) {
            std::cout << " - " << idx.docPath(id) << "\n";
        }
    }

    return 0;
}
