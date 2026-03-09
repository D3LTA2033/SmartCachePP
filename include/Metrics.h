#pragma once
#include <string>

namespace SmartCachePP {

class Metrics {
public:
    Metrics() : hits(0), misses(0), evictions(0) {}

    void recordHit() { ++hits; }
    void recordMiss() { ++misses; }
    void recordEviction() { ++evictions; }

    struct Stats {
        int hits;
        int misses;
        int evictions;
        std::string toString() const {
            return "Hits: " + std::to_string(hits) +
                   ", Misses: " + std::to_string(misses) +
                   ", Evictions: " + std::to_string(evictions);
        }
    };

    Stats getStats() const {
        return Stats{hits, misses, evictions};
    }

private:
    int hits;
    int misses;
    int evictions;
};

}