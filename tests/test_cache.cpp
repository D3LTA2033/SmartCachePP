#include "SmartCache.h"
#include "LRU.h"
#include "Metrics.h"
#include "DiskPersistence.h"
#include "ThreadPool.h"
#include "Logger.h"
#include <cassert>
#include <chrono>
#include <thread>
#include <string>
#include <iostream>

using namespace SmartCachePP;

int main() {
    Logger logger(std::cout, Logger::LogLevel::Debug);
    Metrics metrics;
    LRU<std::string, std::string> cache(2, metrics);

    cache.set("a", "alpha");
    cache.set("b", "bravo");
    assert(cache.get("a").value() == "alpha");
    assert(cache.get("b").value() == "bravo");
    cache.set("c", "charlie");
    assert(!cache.get("a").has_value());
    assert(cache.get("b").value() == "bravo");
    assert(cache.get("c").value() == "charlie");
    assert(metrics.getStats().evictions == 1);

    metrics.recordHit();
    metrics.recordMiss();
    auto stats = metrics.getStats();
    assert(stats.hits > 0);
    assert(stats.misses > 0);

    cache.remove("b");
    assert(!cache.get("b").has_value());
    cache.set("d", "delta");
    cache.set("e", "echo");
    cache.set("f", "foxtrot");
    cache.setExpiry("e", std::chrono::milliseconds(1));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    cache.removeExpired();
    assert(!cache.get("e").has_value());

    cache.set("x", "xray");
    cache.set("y", "yankee");
    DiskPersistence persistence;
    persistence.save(cache.data(), "test_cache_save.dat");

    cache.clear();
    assert(cache.data().empty());
    persistence.load(cache.data(), "test_cache_save.dat");
    assert(!cache.data().empty());
    assert(cache.get("x").value() == "xray");
    assert(cache.get("y").value() == "yankee");

    ThreadPool pool(1);
    bool cleaned = false;
    pool.enqueue([&]() {
        cache.clear();
        cleaned = true;
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    pool.shutdown();
    assert(cleaned);

    std::cout << "All SmartCache++ tests passed.\n";
    return 0;
}