#include "cache.h"

Cache::Cache(int numSets, int blocksPerSet, bool writeAllocate, bool writeBack, bool lruEviction)
    : blocksPerSet(blocksPerSet), writeAllocate(writeAllocate), writeBack(writeBack), 
    lruEviction(lruEviction), currentCycle(0) {
    sets.resize(numSets);

    for (int i = 0; i < numSets; ++i) {
        sets[i].blocks.resize(blocksPerSet);
        for (int j = 0; j < blocksPerSet; ++j) {
            sets[i].blocks[j].valid = false;
            sets[i].blocks[j].tag = 0;
            sets[i].blocks[j].dirty = false;
            sets[i].blocks[j].accessed = 0;
        }
    }

    totalLoads = 0;
    totalStores = 0;
    loadHits = 0;
    loadMisses = 0;
    storeHits = 0;
    storeMisses = 0;
    totalCycles = 0;
}

void Cache::read(unsigned int address, int bytesInBlock) {
    totalLoads++;  // Increment the total number of loads.

    // Decode the address.
    unsigned int index = (address / bytesInBlock) % numSets;  // Extract the set index.
    unsigned int tag = address / (bytesInBlock * numSets);  // Extract the tag.

    Set& currentSet = sets[index];  // Reference the relevant set.

    bool hit = false;  // Whether the read was a cache hit.
    unsigned int lruIndex = 0;  // To keep track of the least recently used block.

    for (unsigned int i = 0; i < blocksPerSet; ++i) {
        if (currentSet.blocks[i].valid && currentSet.blocks[i].tag == tag) {
            // Cache hit!
            hit = true;
            currentSet.blocks[i].accessedCounter = currentCycle;  // Update the accessed counter for LRU.
            loadHits++;  // Increment the number of load hits.
            break;
        }

        // Check for the least recently used block.
        if (currentSet.blocks[i].accessedCounter < currentSet.blocks[lruIndex].accessedCounter) {
            lruIndex = i;
        }
    }

    if (!hit) {
        // Cache miss.
        loadMisses++;  // Increment the number of load misses.
        totalCycles += MEMORY_ACCESS_CYCLES;  // Simulate the time taken to fetch from main memory.

        // Evict the least recently used block (LRU).
        Block& evictedBlock = currentSet.blocks[lruIndex];

        if (evictedBlock.valid && evictedBlock.isDirty) {
            totalCycles += MEMORY_ACCESS_CYCLES;  // Simulate the time taken to write back to memory.
            evictedBlock.isDirty = false;  // Reset the dirty flag.
        }

        // Load the data into the cache.
        evictedBlock.valid = true;
        evictedBlock.tag = tag;
        evictedBlock.accessedCounter = currentCycle;  // Update the accessed counter.
    }

    totalCycles++;  // Simulate the time taken to access the cache.

    // You may want to update the current cycle if you're tracking it.
    currentCycle++;
}


void Cache::write(unsigned int address, int bytesInBlock) {
    // TODO: Implement the write logic based on the cache settings.
    // handle LRU eviction and the accessed counters.
}

void Cache::printSummary() {
    std::cout << "Total loads: " << totalLoads << std::endl;
    std::cout << "Total stores: " << totalStores << std::endl;
    std::cout << "Load hits: " << loadHits << std::endl;
    std::cout << "Load misses: " << loadMisses << std::endl;
    std::cout << "Store hits: " << storeHits << std::endl;
    std::cout << "Store misses: " << storeMisses << std::endl;
    std::cout << "Total cycles: " << totalCycles << std::endl;
}
