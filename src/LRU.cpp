#include "LRU.h"
#include "EvictionPolicy.h"
#include <unordered_map>
#include <list>
#include <cstddef>

namespace SmartCachePP {

class LRU : public EvictionPolicy {
public:
    using Key = std::string;

    explicit LRU(std::size_t capacity) : capacity_(capacity) {}

    void onSet(const Key& key) override {
        auto it = map_.find(key);
        if (it != map_.end()) {
            order_.erase(it->second);
            order_.push_front(key);
            map_[key] = order_.begin();
        } else {
            order_.push_front(key);
            map_[key] = order_.begin();
            if (order_.size() > capacity_) {
                evict();
            }
        }
    }

    void onGet(const Key& key) override {
        auto it = map_.find(key);
        if (it != map_.end()) {
            order_.erase(it->second);
            order_.push_front(key);
            map_[key] = order_.begin();
        }
    }

    std::string evict() override {
        if (order_.empty()) return "";
        auto last = order_.back();
        order_.pop_back();
        map_.erase(last);
        return last;
    }

    bool shouldEvict(std::size_t currentSize) const override {
        return currentSize > capacity_;
    }

private:
    std::size_t capacity_;
    std::list<Key> order_;
    std::unordered_map<Key, std::list<Key>::iterator> map_;
};

}