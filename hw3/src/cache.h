#ifndef CACHE_H
#define CACHE_H

#include <vector>

const int MEMORY_ACCESS_CYCLES = 100;

struct Block {
    bool valid;
    unsigned int tag;
    bool dirty;  // For write-back policy
    unsigned long long accessedCounter;  // LRU counter
};

struct Set {
    std::vector<Block> blocks;
};

class Cache {
private:
    std::vector<Set> sets;
    int numSets;
    int blocksPerSet;
    bool writeAllocate;
    bool writeBack;
    bool lruEviction;

    unsigned long currentCycle;

    unsigned long totalLoads;
    unsigned long totalStores;
    unsigned long loadHits;
    unsigned long loadMisses;
    unsigned long storeHits;
    unsigned long storeMisses;
    unsigned long totalCycles;

public:
    Cache(int numSets, int blocksPerSet, bool writeAllocate, bool writeBack, bool lruEviction);
    void read(unsigned int address, int bytesInBlock);
    void write(unsigned int address, int bytesInBlock);
    void printSummary();
};

#endif // CACHE_H
