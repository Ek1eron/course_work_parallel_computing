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
    std::vector<int> searchPhrase(const std::string& phrase) const;

    const std::string& docPath(int docId) const;

    int totalDocuments() const;
    int totalWords() const;

private:
    std::unordered_map<int, std::string> documents_;
    ConcurrentMap index_;
};
