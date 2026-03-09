#pragma once
#include <string>
#include <unordered_map>
#include "CacheEntry.h"

namespace SmartCachePP {

using Key = std::string;

class EvictionPolicy {
public:
    using Value = std::string;
    virtual ~EvictionPolicy() = default;
    virtual void onSet(const Key& key, const Value& value) = 0;
    virtual void onGet(const Key& key) = 0;
    virtual void evict(std::unordered_map<Key, CacheEntry>& cache) = 0;
};

}