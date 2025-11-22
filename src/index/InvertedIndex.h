#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "../concurrent/ConcurrentMap.h"

class InvertedIndex {
public:
    void addDocument(int docId, const std::string& path);

    void build(int numThreads = 1);

    std::vector<int> search(const std::string& word) const;
    const std::string& docPath(int docId) const;

private:
    std::unordered_map<int, std::string> documents_;
    ConcurrentMap index_;
};
