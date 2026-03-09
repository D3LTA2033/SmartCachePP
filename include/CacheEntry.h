#pragma once
#include <string>
#include <chrono>

namespace SmartCachePP {

using Value = std::string;

struct CacheEntry {
    Value value;
    std::chrono::steady_clock::time_point expireTime;
    int accessCount;
};

}