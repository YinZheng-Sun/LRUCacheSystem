#ifndef __LEI_LRU_CACHE_HASHTABLE_H__
#define __LEI_LRU_CACHE_HASHTABLE_H__
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <string>

struct LRUNode {
    void *value;
    LRUNode *next_hash; // next node in the same bucket
    LRUNode *next;
    LRUNode *prev;

    size_t charge;      // Value length
    size_t key_length;
    uint32_t hash;
    uint32_t refs;
    char key_data[1];   // Beginning of key, is stored in LRUNode, this field must be defined at last

    std::string key() { return std::string(key_data, key_length); }
};

class HashTable {
public:
    HashTable(): m_length(0), m_elements(0), m_list(nullptr) { Resize(); }
    ~HashTable() {
        if (m_list != nullptr) {
            delete []m_list; m_list = nullptr;
        }
    }
    inline uint32_t elements() const { return m_elements; }

    LRUNode* Lookup(const std::string &key, uint32_t hash);
    LRUNode* Insert(LRUNode* n);
    LRUNode* Remove(const std::string &key, uint32_t hash);

private:
    uint32_t m_length;
    uint32_t m_elements;
    LRUNode **m_list;

    void Resize();
    LRUNode** FindPointer(const std::string &key, uint32_t hash);
};

#endif
