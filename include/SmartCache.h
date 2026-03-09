#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <optional>
#include <cstddef>

namespace SmartCachePP {

class EvictionPolicy;
class Metrics;
class ThreadPool;

class SmartCache {
public:
    using Key = std::string;
    using Value = std::string;

    SmartCache(EvictionPolicy* evictionPolicy, Metrics* metrics, ThreadPool* threadPool = nullptr, int ttlSeconds = 0, std::size_t maxSize = 0);

    void set(const Key& key, const Value& value);
    std::optional<Value> get(const Key& key);
    void remove(const Key& key);
    void removeExpired();

private:
    struct CacheEntry {
        Value value;
        std::chrono::steady_clock::time_point createdAt;
        std::optional<std::chrono::seconds> ttl;
    };

    std::unordered_map<Key, CacheEntry> cache_;
    EvictionPolicy* evictionPolicy;
    Metrics* metrics;
    ThreadPool* threadPool;
    int ttlSeconds_;
    std::size_t maxSize_;
    std::mutex mutex_;
};

}