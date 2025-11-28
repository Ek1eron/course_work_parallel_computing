#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <mutex>

class ConcurrentMap
{
public:
    explicit ConcurrentMap(size_t shards = 32);

    void add(const std::string& word, int docId);

    std::vector<int> get(const std::string& word) const;

    size_t size() const;

    void clear();

private:
    struct Shard
    {
        mutable std::mutex mtx;
        std::unordered_map<std::string, std::unordered_set<int>> map;
    };

    std::vector<Shard> shards_;

    size_t shardIndex(const std::string& key) const;
};
