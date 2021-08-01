#ifndef __LEI_LRU_CACHE_CACHE_H__
#define __LEI_LRU_CACHE_CACHE_H__
#include <string>

class Cache {
public:
    Cache() {};
    virtual ~Cache() {};
    // No copying allowed
    Cache(const Cache&) = delete;
    void operator=(const Cache&) = delete;

    struct Node {};

    virtual Node* Insert(const std::string &key, const void *value, size_t charge,
                         void (*deleter)(const std::string &key, void *value)) = 0;
    virtual Node* Lookup(const std::string &key) = 0;
    virtual void Release(Node *n) = 0;
    virtual void Earse(const std::string &key) = 0;

private:
    void LRU_Remove(Node *n);
    void LRU_Append(Node *n);
    void Unref(Node *n);
};

#endif
