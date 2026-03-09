#pragma once
#include <unordered_map>
#include "EvictionPolicy.h"
#include "CacheEntry.h"
#include "Key.h"
#include "Value.h"

namespace SmartCachePP {
class LRU : public EvictionPolicy {
public:
    void onSet(const Key& key, const Value& value) override;
    void onGet(const Key& key) override;
    Key evict(std::unordered_map<Key, CacheEntry>& cache) override;
};
}