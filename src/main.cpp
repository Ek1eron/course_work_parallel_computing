#include <iostream>
#include <filesystem>
#include "index/InvertedIndex.h"
#include "server/SearchServer.h"

namespace fs = std::filesystem;

int main() {
    InvertedIndex idx;

    int docId = 0;
    for (auto& p : fs::recursive_directory_iterator("data")) {
        if (p.is_regular_file() && p.path().extension() == ".txt")
            idx.addDocument(docId++, p.path().string());
    }

    std::cout << "Loaded " << docId << " documents\n";

    idx.build(32);

    SearchServer server(idx, 8080, 8);
    server.run();

    return 0;
}
