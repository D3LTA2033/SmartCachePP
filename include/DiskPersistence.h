#pragma once
#include <string>
#include <unordered_map>
#include "CacheEntry.h"

namespace SmartCachePP {

using Key = std::string;

class DiskPersistence {
public:
    void save(const std::unordered_map<Key, CacheEntry>& cache, const std::string& filename);
    void load(std::unordered_map<Key, CacheEntry>& cache, const std::string& filename);
};

}