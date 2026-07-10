#ifndef LRU_CACHE_HPP
#define LRU_CACHE_HPP

#include <list>
#include <unordered_map>
#include <string>
#include <sstream>

using namespace std;

template<typename K, typename V>
class LRUCache {
public:
    LRUCache(size_t capacity) : _capacity(capacity) {}

    void put(const K& key, const V& value) {
        auto it = _map.find(key);
        if (it != _map.end()) {
            _cache.splice(_cache.begin(), _cache, it->second);
            it->second->second = value;
            return;
        }

        if (_cache.size() == _capacity) {
            K last_key = _cache.back().first;
            _cache.pop_back();
            _map.erase(last_key);
        }

        _cache.emplace_front(key, value);
        _map[key] = _cache.begin();
    }

    string getDisplayString(const string& userName) const {
        stringstream ss;
        ss << "--- Recent Payees for " << userName << " (LRU Cache) ---\n";
        if (_cache.empty()) {
            ss << "No cached payees found.\n";
            return ss.str();
        }
        for (const auto& pair : _cache) {
            ss << "  - Account #" << pair.first << " (Name: " << pair.second << ")\n";
        }
        return ss.str();
    }

private:
    size_t _capacity;
    list<pair<K, V>> _cache;
    unordered_map<K, typename list<pair<K, V>>::iterator> _map;
};

#endif