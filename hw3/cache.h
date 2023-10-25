#ifndef CACHE_H
#define CACHE_H

#include <vector>

const int MEMORY_ACCESS_CYCLES = 100;

struct Block {
    bool valid;
    unsigned int tag;
    bool dirty;  // For write-back policy
    unsigned long accessedCounter;  // LRU counter
    unsigned long loadCounter;  // FIFO counter
};

struct Set {
    std::vector<Block> blocks;
    int evictionIndex = 0;
};

class Cache {
private:
    std::vector<Set> sets;
    int numSets;
    int blocksPerSet;
    bool writeAllocate;
    bool writeBack;
    bool lruEviction;
    bool fifoEviction;

    unsigned long currentCycle;

    unsigned long totalLoads;
    unsigned long totalStores;
    unsigned long loadHits;
    unsigned long loadMisses;
    unsigned long storeHits;
    unsigned long storeMisses;
    unsigned long totalCycles;

public:
    Cache(int numSets, int blocksPerSet, bool writeAllocate, bool writeBack, bool lruEviction, bool fifoEviction);
    void read(unsigned int address, int bytesInBlock);
    void write(unsigned int address, int bytesInBlock);
    void replaceBlock(unsigned int evictionIndex, Set& currentSet, char command, int bytesInBlock,  unsigned int tag);
    void printSummary();
};

#endif // CACHE_H
