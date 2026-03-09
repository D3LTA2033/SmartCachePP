#include "SmartCache.h"
#include "EvictionPolicy.h"
#include "Metrics.h"
#include "ThreadPool.h"
#include <unordered_map>
#include <mutex>
#include <memory>
#include <chrono>
#include <optional>

namespace SmartCachePP {

class SmartCache {
public:
    using Key = std::string;
    using Value = std::string;

    SmartCache(std::shared_ptr<EvictionPolicy> policy,
               std::shared_ptr<Metrics> metrics,
               std::shared_ptr<ThreadPool> threadPool = nullptr)
        : policy_(std::move(policy)), metrics_(std::move(metrics)), threadPool_(std::move(threadPool)) {}

    void set(const Key& key, const Value& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto now = std::chrono::steady_clock::now();
        cache_[key] = CacheEntry{value, now, std::nullopt};
        if (policy_) policy_->onSet(key);
        evictIfNecessary();
    }

    std::optional<Value> get(const Key& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            if (!isExpired(it->second)) {
                if (policy_) policy_->onGet(key);
                if (metrics_) metrics_->recordHit();
                return it->second.value;
            }
            cache_.erase(it);
            if (metrics_) metrics_->recordMiss();
            return std::nullopt;
        }
        if (metrics_) metrics_->recordMiss();
        return std::nullopt;
    }

    void remove(const Key& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        cache_.erase(key);
    }

    void removeExpired() {
        auto cleanup = [this] {
            std::lock_guard<std::mutex> lock(mutex_);
            for (auto it = cache_.begin(); it != cache_.end(); ) {
                if (isExpired(it->second)) it = cache_.erase(it);
                else ++it;
            }
        };
        if (threadPool_) threadPool_->enqueue(cleanup);
        else cleanup();
    }

private:
    struct CacheEntry {
        Value value;
        std::chrono::steady_clock::time_point createdAt;
        std::optional<std::chrono::seconds> ttl;
    };

    std::unordered_map<Key, CacheEntry> cache_;
    std::shared_ptr<EvictionPolicy> policy_;
    std::shared_ptr<Metrics> metrics_;
    std::shared_ptr<ThreadPool> threadPool_;
    std::mutex mutex_;

    bool isExpired(const CacheEntry& entry) const {
        if (!entry.ttl) return false;
        auto now = std::chrono::steady_clock::now();
        return now - entry.createdAt > *entry.ttl;
    }

    void evictIfNecessary() {
        if (!policy_) return;
        while (policy_->shouldEvict(cache_.size())) {
            auto toEvict = policy_->evict();
            if (cache_.erase(toEvict) && metrics_) metrics_->recordEviction();
        }
    }
};

}