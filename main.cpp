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
#include <vector>

#define NUM_THREADS 8
using namespace std;

void* test(LRUCacheSystem *s)
{
    int count = 0;
    for (int i=1; i < s->Total_blocks_ - 1; i++) {   //每个线程的写操作为  48*3 = 144
        LRUNode *n = s->Seek(rand()%50+1);
        s->Put(n,"nb1");
        s->Release(n);

        LRUNode *n2 = s->Seek(rand()%50+1);
        s->Put(n2,"nb2");
        s->Release(n2);

        LRUNode *n3 = s->Seek(rand()%50+1);
        s->Put(n3,"nb3");
        s->Release(n3);
        count += 3;
    }
    cout << count <<endl;

    return 0;
}

int random(int blocks) {

}


int main(int argc, char **argv) {
    LRUCacheSystem *s = new LRUCacheSystem("/Users/sunyinzheng/lru.txt");
    vector<std::thread> threads;
    clock_t start,end;
    start = clock();
    for (int i = 0; i < 9; i++) {
        threads.push_back(std::thread(test, std::ref(s)));
    }
    for (auto& th : threads) {
        th.join();
    }
    end = clock();
    double time = double(end-start)/CLOCKS_PER_SEC;
    cout<<"time = "<<time<<"s"<<endl;
    cout<<"options per second = "<<144*9/time<<" ops"<<endl;
    delete s;
    return 0;
}

