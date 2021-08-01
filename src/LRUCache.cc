#include <iostream>
#include "LRUCache.h"
#include "cache.h"

using namespace std;

Cache* NewLRUCache(size_t capacity) {
    return new SharedLRUCache(capacity);
}

LRUCache::LRUCache()
        : m_capacity(0),
          m_usage(0),
          m_last_id(0) {
    // Make empty circular linked list
    m_lru.next = &m_lru;
    m_lru.prev = &m_lru;
}

LRUCache::~LRUCache() {
    LRUNode *n = nullptr;
    while ((n = m_lru.next) != &m_lru) {
        assert(n->refs == 1);
        m_lru.next = n->next;
        LRU_Remove(n);
        Unref(n);
    }
}

void LRUCache::LRU_Append(LRUNode *n)  {
    // Make "n" newest entry by inserting just before m_lru
    n->prev = m_lru.prev;
    n->next = &m_lru;
    m_lru.prev->next = n;
    m_lru.prev = n;
}

// Remove from list, but not free the memory
void LRUCache::LRU_Remove(LRUNode *n) {
    n->prev->next = n->next;
    n->next->prev = n->prev;
}

void LRUCache::Unref(LRUNode *n) {
    assert(n->refs > 0);
    --(n->refs);
    if (n->refs <= 0) {
        m_usage -= n->charge;
        // TODO add deleter
        free(n); n = nullptr;
    }
}

Cache::Node* LRUCache::Lookup(const string& key, uint32_t hash) {
    std::lock_guard<std::mutex> guard(m_mutex);
    LRUNode *n = m_table.Lookup(key, hash);
    if (n != nullptr) {
        LRU_Remove(n);
        LRU_Append(n);
        n->refs++;
    }
    return reinterpret_cast<Cache::Node*>(n);
}

Cache::Node* LRUCache::Insert(const std::string& key, uint32_t hash, const void* value, size_t charge,
                              void (*deleter)(const string& key, void* value)) {
    LRUNode *n = static_cast<LRUNode*>(malloc(sizeof(LRUNode)-1 + key.size()));
    n->value = const_cast<void*>(value);
    n->charge = charge;
    n->hash = hash;
    n->key_length = key.size();
    n->refs = 2;    // One from LRUCache, one for the returned handle
    memcpy(n->key_data, key.data(), key.size());

    std::lock_guard<std::mutex> guard(m_mutex);
    LRU_Append(n);
    m_usage += charge;

    LRUNode *old = m_table.Insert(n);
    if (old != nullptr) {
        LRU_Remove(old);
        Unref(old);
    }

    while (m_usage > m_capacity && m_lru.next != &m_lru) {
        LRUNode *oldest = m_lru.next;
        LRU_Remove(oldest);
        m_table.Remove(oldest->key(), oldest->hash);
        Unref(oldest);
    }
    return reinterpret_cast<Cache::Node*>(n);
}

void LRUCache::Earse(const string &key, uint32_t hash) {
    std::lock_guard<std::mutex> guard(m_mutex);
    LRUNode *n = m_table.Remove(key, hash);
    if (n != nullptr) {
        LRU_Remove(n);
        Unref(n);
    }
}

void LRUCache::Release(LRUNode *n) {
    std::lock_guard<std::mutex> guard(m_mutex);
    Unref(n);
}

SharedLRUCache::SharedLRUCache(size_t capacity) {
    // capacity + (kNumShards - 1) 为了防止capacity比kNumShards小，per_shard为0
    const size_t size_per_shard = (capacity + (kNumShards - 1)) / kNumShards;
    for (int i = 0; i < kNumShards; i++) {
        m_shard[i].set_capacity(size_per_shard);
    }
}

Cache::Node* SharedLRUCache::Insert(const std::string &key, const void *value, size_t charge,
                                    void (*deleter)(const std::string &key, void *value)) {
    uint32_t hash = HashString(key);
    uint32_t shard = Shard(hash);
    return m_shard[shard].Insert(key, hash, value, charge, deleter);
}

Cache::Node* SharedLRUCache::Lookup(const std::string &key) {
    uint32_t hash = HashString(key);
    uint32_t shard = Shard(hash);
    return m_shard[shard].Lookup(key, hash);
}

void SharedLRUCache::Release(Node *node) {
    LRUNode *n = reinterpret_cast<LRUNode*>(node);
    uint32_t shard = Shard(n->hash);
    m_shard[shard].Release(n);
}

void SharedLRUCache::Earse(const std::string &key) {
    uint32_t hash = HashString(key);
    uint32_t shard = Shard(hash);
    m_shard[shard].Earse(key, hash);
}
