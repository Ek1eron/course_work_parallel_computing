#include "ConcurrentMap.h"
#include <functional>

ConcurrentMap::ConcurrentMap(size_t shards) : shards_(shards) {}

size_t ConcurrentMap::shardIndex(const std::string& key) const {
    return std::hash<std::string>{}(key) % shards_.size();
}

void ConcurrentMap::add(const std::string& word, int docId) {
    auto& shard = shards_[shardIndex(word)];
    std::lock_guard<std::mutex> lock(shard.mtx);
    shard.map[word].insert(docId);
}

std::vector<int> ConcurrentMap::get(const std::string& word) const {
    auto& shard = shards_[shardIndex(word)];
    std::lock_guard<std::mutex> lock(shard.mtx);

    auto it = shard.map.find(word);
    if (it == shard.map.end()) return {};

    return std::vector<int>(it->second.begin(), it->second.end());
}

size_t ConcurrentMap::size() const {
    size_t total = 0;
    for (auto& shard : shards_) {
        std::lock_guard<std::mutex> lock(shard.mtx);
        total += shard.map.size();
    }
    return total;
}

void ConcurrentMap::clear() {
    for (auto& shard : shards_) {
        std::lock_guard<std::mutex> lock(shard.mtx);
        shard.map.clear();
    }
}
