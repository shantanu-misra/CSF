#include "cache.h"
#include <iostream>

// Constructor to initialize cache parameters and counters.
Cache::Cache(int numSets, int blocksPerSet, bool writeAllocate, bool writeBack, bool lruEviction, bool fifoEviction)
    : numSets(numSets), blocksPerSet(blocksPerSet), writeAllocate(writeAllocate), 
      writeBack(writeBack), lruEviction(lruEviction), fifoEviction(fifoEviction), currentCycle(0) {
    sets.resize(numSets);

    // Initializing each block in each set.
    for (int i = 0; i < numSets; ++i) {
        sets[i].blocks.resize(blocksPerSet);
        for (int j = 0; j < blocksPerSet; ++j) {
            sets[i].blocks[j].valid = false;  // Initially, block contains no valid data.
            sets[i].blocks[j].tag = 0;
            sets[i].blocks[j].dirty = false;  // No pending writes.
            sets[i].blocks[j].accessedCounter = 0;  // For LRU.
        }
    }

    // Resetting all counters to 0.
    totalLoads = 0;
    totalStores = 0;
    loadHits = 0;
    loadMisses = 0;
    storeHits = 0;
    storeMisses = 0;
    totalCycles = 0;
}

// Function to handle a memory read request.
void Cache::read(unsigned int address, int bytesInBlock) {
    totalLoads++;

    // Extracting set index and tag from the address.
    unsigned int index = (address / bytesInBlock) % numSets;
    unsigned int tag = address / (bytesInBlock * numSets);

    Set& currentSet = sets[index];

    bool hit = false;
    unsigned int lruIndex = 0;

    // Check each block in the set to find a hit or identify the LRU block.
    for (int i = 0; i < blocksPerSet; ++i) {
        if (currentSet.blocks[i].valid && currentSet.blocks[i].tag == tag) {
            hit = true;
            if (lruEviction) {
                currentSet.blocks[i].accessedCounter = currentCycle;  // LRU policy update.
            }
            loadHits++;
            break;
        }

        if (currentSet.blocks[i].accessedCounter < currentSet.blocks[lruIndex].accessedCounter) {
            lruIndex = i;  // Track the least recently accessed block.
        }
    }

    // If miss, we might need to evict and access main memory.
    if (!hit) {
        loadMisses++;
        totalCycles += MEMORY_ACCESS_CYCLES;  // Penalty for main memory access.

        Block& evictedBlock = currentSet.blocks[lruIndex];

        // If block is dirty, write back to main memory.
        if (evictedBlock.valid && evictedBlock.dirty) {
            totalCycles += MEMORY_ACCESS_CYCLES;
            evictedBlock.dirty = false;
        }

        // Update block with new data.
        evictedBlock.valid = true;
        evictedBlock.tag = tag;
        evictedBlock.accessedCounter = currentCycle;  // Set block as recently accessed.
    }

    totalCycles++;
    currentCycle++;  // Simulating time progression.
}

// Function to handle a memory write request.
void Cache::write(unsigned int address, int bytesInBlock) {
    totalStores++;

    unsigned int index = (address / bytesInBlock) % numSets;
    unsigned int tag = address / (bytesInBlock * numSets);

    Set& currentSet = sets[index];

    bool hit = false;
    unsigned int lruIndex = 0;

    // Check each block in the set for a hit or identify the LRU block.
    for (int i = 0; i < blocksPerSet; ++i) {
        if (currentSet.blocks[i].valid && currentSet.blocks[i].tag == tag) {
            hit = true;
            if (lruEviction) {
                currentSet.blocks[i].accessedCounter = currentCycle;  // LRU policy update.
            }
            storeHits++;
            break;
        }

        if (currentSet.blocks[i].accessedCounter < currentSet.blocks[lruIndex].accessedCounter) {
            lruIndex = i;
        }
    }

    // If miss, similar steps as in read but more nuances based on write policies.
    if (!hit) {
        storeMisses++;
        totalCycles += MEMORY_ACCESS_CYCLES;

        Block& evictedBlock = currentSet.blocks[lruIndex];
        
        if (evictedBlock.valid && evictedBlock.dirty) {
            totalCycles += MEMORY_ACCESS_CYCLES;
            evictedBlock.dirty = false;
        }

        evictedBlock.valid = true;
        evictedBlock.tag = tag;
        evictedBlock.accessedCounter = currentCycle;
    }

    totalCycles++;
    currentCycle++;
}

// Function to print cache stats at the end.
void Cache::printSummary() {
    std::cout << "Total loads: " << totalLoads << std::endl;
    std::cout << "Total stores: " << totalStores << std::endl;
    std::cout << "Load hits: " << loadHits << std::endl;
    std::cout << "Load misses: " << loadMisses << std::endl;
    std::cout << "Store hits: " << storeHits << std::endl;
    std::cout << "Store misses: " << storeMisses << std::endl;
    std::cout << "Total cycles: " << totalCycles << std::endl;
}
