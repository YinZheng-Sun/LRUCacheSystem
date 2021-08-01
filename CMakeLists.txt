cmake_minimum_required(VERSION 3.15)
project(LRUCacheSystem)

set(CMAKE_CXX_STANDARD 14)

add_executable(LRUCacheSystem main.cpp src/LRUCache.cc src/LRUCache.h src/cache.h src/hashtable.cc src/hashtable.h Murmur3/Murmur3.cc Murmur3/Murmur3.h src/LRUCacheSystem.cc src/LRUCacheSystem.h)