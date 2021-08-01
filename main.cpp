#include <iostream>

#include <iostream>
#include <string>
#include <algorithm>  //for std::generate_n
#include <stdlib.h>
#include <iostream>
#include "src/LRUCache.h"
#include "src/LRUCacheSystem.h"
#include <pthread.h>
#include <thread>
#include <time.h>
#define NUM_THREADS 8
using namespace std;

std::string random_string( size_t length )
{
    auto randchar = []() -> char
    {
        const char charset[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}

void TestCache(Cache *cache) {
    srand(time(NULL));
    string random_key = random_string(13);
    string random_value = random_string(29);
    printf("random_key: %s\n", random_key.data());
    printf("random_value: %s\n", random_value.data());
    Cache::Node *n = cache->Insert("1", random_value.data(), random_value.length(), nullptr);
    assert(n != nullptr);
    LRUNode *ln = reinterpret_cast<LRUNode*>(n);
    assert(ln->key() == "1");
//    printf("key: %s\n", ln->key().c_str());
    assert(ln->refs == 2);
    cache->Release(n);
    assert(ln->refs == 1);

    n = cache->Lookup("1");
    ln = reinterpret_cast<LRUNode*>(n);
    printf("key: %s\n", ln->key().data());
    printf("value: %s\n", static_cast<char*>(ln->value));
    cache->Release(n);

    string new_value = random_string(29);
    n = cache->Insert(random_key, new_value.data(), new_value.length(), nullptr);
    cache->Release(n);

    n = cache->Lookup(random_key);
    ln = reinterpret_cast<LRUNode*>(n);
    printf("key: %s\n", ln->key().data());
    printf("new value: %s\n", static_cast<char*>(ln->value));
    cache->Release(n);

    cache->Earse(random_key);
    n = cache->Lookup(random_key);
    assert(n == nullptr);
}

void* test(LRUCacheSystem *s)
{
    for (int i=1; i < 50; i++) {
        LRUNode *n = s->Seek(i);
        std::cout << (char *) n->value << std::endl;
        fflush(stdout);
        s->Release(n);
    }

    return 0;
}

int main(int argc, char **argv) {
//    const size_t kCacheCapacity = 10*1024;
    LRUCacheSystem *s = new LRUCacheSystem("/Users/sunyinzheng/lru.txt");
    LRUNode *n = s->Seek(1);
    std::cout << (char *) n->value << std::endl;
    s->Release(n);

    LRUNode *n2 = s->Put(n,"wowoefa");
    std::cout << (char *) n2->value << std::endl;
    s->Release(n2);
       
    LRUNode *n3 = s->Seek(1);
    std::cout << (char *) n3->value << std::endl;
    s->Release(n3);
//    int count = 0;
//    clock_t start,end;
//    start = clock();
//    std::thread t1(test,std::ref(s));
//    std::thread t2(test,std::ref(s));
//    std::thread t3(test,std::ref(s));
//    std::thread t4(test,std::ref(s));
//    std::thread t5(test,std::ref(s));
//    std::thread t6(test,std::ref(s));
//    std::thread t7(test,std::ref(s));
//    std::thread t8(test,std::ref(s));

//    t1.join();
//    t2.join();t3.join();t4.join();t5.join();t6.join();t7.join();t8.join();
//    end = clock();
//    cout<<"time = "<<double(end-start)/CLOCKS_PER_SEC<<"s"<<endl;
//    delete s;
    return 0;
}
