#include "SmartCache.h"
#include "LRU.h"
#include "Metrics.h"
#include "DiskPersistence.h"
#include "ThreadPool.h"
#include "Logger.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

using namespace SmartCachePP;

int main() {
    Logger logger(std::cout, Logger::LogLevel::Info);
    Metrics metrics;
    LRU lruPolicy;
    SmartCache<std::string, std::string> cache(3, &lruPolicy, &metrics, &logger);

    cache.set("a", "alpha");
    cache.set("b", "bravo");
    cache.set("c", "charlie");

    auto val1 = cache.get("a");
    if (val1) {
        logger.logInfo("Got a: " + *val1);
        metrics.recordHit();
    } else {
        metrics.recordMiss();
    }

    auto val2 = cache.get("b");
    if (val2) {
        logger.logInfo("Got b: " + *val2);
        metrics.recordHit();
    } else {
        metrics.recordMiss();
    }

    cache.set("d", "delta");

    auto val3 = cache.get("c");
    if (val3) {
        logger.logInfo("Got c: " + *val3);
        metrics.recordHit();
    } else {
        logger.logInfo("Key c was evicted (as expected for LRU)");
        metrics.recordMiss();
    }

    auto val4 = cache.get("a");
    if (val4) {
        logger.logInfo("Got a: " + *val4);
        metrics.recordHit();
    } else {
        logger.logInfo("Key a was evicted (unexpected)");
        metrics.recordMiss();
    }

    logger.logInfo("Cache Metrics: " + metrics.getStats().toString());

    DiskPersistence persistence;
    persistence.save(cache.data(), "cache.dat");
    cache.clear();
    persistence.load(cache.data(), "cache.dat");
    logger.logInfo("Cache loaded from disk. Size: " + std::to_string(cache.data().size()));

    ThreadPool pool(2);
    pool.enqueue([&cache, &logger]() {
        cache.clear();
        logger.logInfo("Async cleanup complete.");
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    pool.shutdown();

    return 0;
}