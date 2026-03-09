#include "Metrics.h"
#include <sstream>

namespace SmartCachePP {

void Metrics::recordHit() {
    ++hits_;
}

void Metrics::recordMiss() {
    ++misses_;
}

void Metrics::recordEviction() {
    ++evictions_;
}

std::string Metrics::getStats() const {
    std::ostringstream oss;
    oss << "Hits: " << hits_ << ", Misses: " << misses_ << ", Evictions: " << evictions_;
    return oss.str();
}

}