#include "DiskPersistence.h"
#include "SmartCache.h"
#include <fstream>
#include <string>
#include <unordered_map>

namespace SmartCachePP {

void DiskPersistence::save(const std::unordered_map<std::string, SmartCache::CacheEntry>& cache, const std::string& filename) {
    std::ofstream ofs(filename, std::ios::binary);
    std::size_t size = cache.size();
    ofs.write(reinterpret_cast<const char*>(&size), sizeof(size));
    for (const auto& [key, entry] : cache) {
        std::size_t keyLen = key.size();
        ofs.write(reinterpret_cast<const char*>(&keyLen), sizeof(keyLen));
        ofs.write(key.data(), keyLen);

        std::size_t valueLen = entry.value.size();
        ofs.write(reinterpret_cast<const char*>(&valueLen), sizeof(valueLen));
        ofs.write(entry.value.data(), valueLen);

        auto created = entry.createdAt.time_since_epoch().count();
        ofs.write(reinterpret_cast<const char*>(&created), sizeof(created));

        bool hasTtl = entry.ttl.has_value();
        ofs.write(reinterpret_cast<const char*>(&hasTtl), sizeof(hasTtl));
        if (hasTtl) {
            auto ttlSec = entry.ttl->count();
            ofs.write(reinterpret_cast<const char*>(&ttlSec), sizeof(ttlSec));
        }
    }
}

void DiskPersistence::load(std::unordered_map<std::string, SmartCache::CacheEntry>& cache, const std::string& filename) {
    std::ifstream ifs(filename, std::ios::binary);
    std::size_t size = 0;
    ifs.read(reinterpret_cast<char*>(&size), sizeof(size));
    cache.clear();
    for (std::size_t i = 0; i < size; ++i) {
        std::size_t keyLen = 0;
        ifs.read(reinterpret_cast<char*>(&keyLen), sizeof(keyLen));
        std::string key(keyLen, '\0');
        ifs.read(&key[0], keyLen);

        std::size_t valueLen = 0;
        ifs.read(reinterpret_cast<char*>(&valueLen), sizeof(valueLen));
        std::string value(valueLen, '\0');
        ifs.read(&value[0], valueLen);

        int64_t createdRaw = 0;
        ifs.read(reinterpret_cast<char*>(&createdRaw), sizeof(createdRaw));
        SmartCache::CacheEntry entry;
        entry.value = std::move(value);
        entry.createdAt = std::chrono::steady_clock::time_point(std::chrono::steady_clock::duration(createdRaw));

        bool hasTtl = false;
        ifs.read(reinterpret_cast<char*>(&hasTtl), sizeof(hasTtl));
        if (hasTtl) {
            int64_t ttlSec = 0;
            ifs.read(reinterpret_cast<char*>(&ttlSec), sizeof(ttlSec));
            entry.ttl = std::chrono::seconds(ttlSec);
        } else {
            entry.ttl = std::nullopt;
        }

        cache.emplace(std::move(key), std::move(entry));
    }
}

}