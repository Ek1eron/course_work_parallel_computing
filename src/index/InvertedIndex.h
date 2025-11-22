#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class InvertedIndex {
public:
    void addDocument(int docId, const std::string& path);
    void build();
    std::vector<int> search(const std::string& word) const;

    const std::string& docPath(int docId) const;

private:
    std::unordered_map<int, std::string> documents_;
    std::unordered_map<std::string, std::unordered_set<int>> index_;
};
