# SmartCachePP

**License: MIT**  
Distributed under the MIT License. See [LICENSE](LICENSE) for details.

> Made by D3LTA2033 and [@mcs.s](https://discord.com/users/mcs.s) on Discord

---

## Project Overview

**SmartCachePP** is a high-performance, modular caching library for C++. It supports LRU (Least Recently Used) eviction, metrics tracking, optional disk persistence, and asynchronous cleanup with a thread pool. SmartCachePP is designed for thread-safe operations and easy integration into any modern C++ project.

## Features

- **High performance caching**
- **LRU eviction** support to automatically remove least recently used items
- **Metrics tracking** including hit, miss, and eviction counts
- **Optional disk persistence** for saving and restoring cache state
- **Asynchronous cleanup** using a configurable thread pool
- **Thread-safe** operations
- **Easy to integrate** in C++ projects

## Installation Instructions

Clone the repository and build using CMake:

```bash
git clone https://github.com/D3LTA2033/SmartCachePP.git
cd SmartCachePP
mkdir build
cd build
cmake ..
make
```

## Usage Example

Here's a minimal C++ example showing key features:

```cpp
#include <iostream>
#include "SmartCache.h"
#include "Metrics.h"
#include "DiskPersistence.h"
#include "ThreadPool.h"
#include "Logger.h"

using namespace SmartCachePP;

int main() {
    SmartCache<std::string, std::string> cache(3); // max 3 items, LRU eviction
    Metrics metrics;
    Logger logger;

    cache.set("a", "alpha");
    cache.set("b", "bravo");
    cache.set("c", "charlie");

    // LRU eviction: adding "d" will evict "a"
    cache.set("d", "delta");

    if (auto val = cache.get("a")) {
        logger.logInfo("Got a: " + *val);
        metrics.recordHit();
    } else {
        logger.logInfo("Key a evicted as expected (LRU).");
        metrics.recordMiss();
    }

    // Track metrics
    std::cout << "Cache hits: " << metrics.getStats().hits << std::endl;
    std::cout << "Evictions: " << metrics.getStats().evictions << std::endl;

    // Save to disk
    DiskPersistence persistence;
    persistence.save(cache.data(), "cache.dat");

    // Clear and reload
    cache.clear();
    persistence.load(cache.data(), "cache.dat");

    // Async cleanup
    ThreadPool pool(2);
    pool.enqueue([&cache]() {
        cache.clear();
        std::cout << "Async cleanup completed." << std::endl;
    });
    pool.shutdown();

    return 0;
}
```

## API Overview

### **SmartCache\<K, V\>**
- `set(key, value)`: Store a value with key, managing LRU eviction
- `get(key)`: Retrieve a value, returns `std::optional<V>`
- `remove(key)`: Remove a specific key
- `clear()`: Clear the entire cache
- `setExpiry(key, duration)`: Set an expiry for a key (optional)
- `removeExpired()`: Remove expired cache entries
- `data()`: Access underlying cache storage

### **LRU**
- Internal mechanism for LRU eviction, used in `SmartCache`

### **Metrics**
- `recordHit()`: Record a cache hit
- `recordMiss()`: Record a cache miss
- `getStats()`: Get hit, miss, and eviction counts

### **DiskPersistence**
- `save(map, filename)`: Save cache to disk
- `load(map, filename)`: Load cache from disk

### **ThreadPool**
- `ThreadPool(size_t numThreads)`: Initialize thread pool
- `enqueue(task)`: Add a task to the pool
- `shutdown()`: Gracefully stop the thread pool

### **Logger**
- `logInfo(message)`, `logError(message)`, `logDebug(message)`: Logging utilities

## Build Instructions

1. **Clone the repository:**
   ```bash
   git clone https://github.com/yourusername/SmartCachePP.git
   cd SmartCachePP
   ```

2. **Configure with CMake:**
   ```bash
   mkdir build
   cd build
   cmake ..
   ```

3. **Build all targets (library, examples, and tests):**
   ```bash
   make
   ```

4. **Run example or tests:**
   ```bash
   ./smartcache_example    # Run usage example
   ./smartcache_test       # Run unit tests
   ```

Include directories and sources are already managed by the provided CMake configuration.
