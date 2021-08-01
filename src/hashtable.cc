#include "hashtable.h"

LRUNode* HashTable::Lookup(const std::string &key, uint32_t hash) {
    return *FindPointer(key, hash);
}

// Return: if hashtable already contains key of n, return the old node, else return nullptr
LRUNode* HashTable::Insert(LRUNode *n) {
    LRUNode **ptr = FindPointer(n->key(), n->hash);
    LRUNode *old = *ptr;
    n->next_hash = (old == nullptr) ? nullptr : old->next_hash;
    *ptr = n;
    if (old == nullptr) {
        if ((++m_elements) > m_length) {
            Resize();
        }
    }
    return old;
}

LRUNode* HashTable::Remove(const std::string &key, uint32_t hash) {
    LRUNode **ptr = FindPointer(key, hash);
    LRUNode *find = *ptr;
    if (find != nullptr) {
        *ptr = find->next_hash;
        --m_elements;
    }
    return find;
}

void HashTable::Resize() {
    uint32_t new_length = 4;
    while (new_length < m_elements) {
        new_length *= 2;
    }
    LRUNode **new_list = new LRUNode*[new_length];
    memset(new_list, 0, sizeof(new_list[0]) * new_length);
    uint32_t count = 0;
    for (uint32_t i = 0; i < m_length; i++) {
        LRUNode *h = m_list[i];
        while (h != nullptr) {
            LRUNode *next = h->next_hash;
            uint32_t hash = h->hash;
            LRUNode **ptr = &new_list[hash & (new_length - 1)]; // find the bucket
            h->next_hash = *ptr;
            *ptr = h;
            h = next;
            count++;
        }
    }
    assert(count == m_elements);
    delete []m_list;
    m_list = new_list;
    m_length = new_length;
}

// return: ptr point to next_hash(and next_hash point to the node match key)
LRUNode** HashTable::FindPointer(const std::string &key, uint32_t hash) {
    LRUNode **ptr  = &m_list[hash & (m_length - 1)];
    while (*ptr != nullptr &&
           ( (*ptr)->key() != key || (*ptr)->hash != hash)) {
        ptr = &(*ptr)->next_hash;
    }
    return ptr;
}
