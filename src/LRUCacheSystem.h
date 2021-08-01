//
// Created by 孙印政 on 2021/7/25.
//

#ifndef LRUCACHE_2_LRUCACHESYSTEM_H
#define LRUCACHE_2_LRUCACHESYSTEM_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <iostream>
#include <fstream>
#include "cache.h"
#include "hashtable.h"


class Block {
public:
    Block();
    ~Block();
    uint32_t block_id;
    bool updated = false;
private:
    const char* data_;
    const size_t length_;
};


class LRUCacheSystem {
public:
    LRUCacheSystem(std::string filepath);
    ~LRUCacheSystem();
    LRUNode* Seek(const uint32_t block_id);
    void Release(LRUNode *n);
    LRUCacheSystem(const LRUCacheSystem&) = delete;
    void operator=(const LRUCacheSystem&) = delete;
public:
    uint32_t Total_blocks_;
    size_t file_size_;
    Cache* cache_;
//    const char *filepath_;
};


#endif //LRUCACHE_2_LRUCACHESYSTEM_H
