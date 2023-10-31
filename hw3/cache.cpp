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
            sets[i].blocks[j].loadCounter = 0;  // For FIFO.
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
    char command = 'r';

    // Extracting set index and tag from the address.
    unsigned int index = (address / bytesInBlock) % numSets;
    unsigned int tag = address / (bytesInBlock * numSets);

    Set& currentSet = sets[index];

    bool hit = false;
    unsigned int evictionIndexCur = 0;

    // Check each block in the set to find a hit or identify the LRU block.
    for (int i = 0; i < blocksPerSet; ++i) {
        if (currentSet.blocks[i].valid && currentSet.blocks[i].tag == tag) {
            hit = true;
            loadHits++;
            if (lruEviction) {
                currentSet.blocks[i].accessedCounter = currentCycle;  // LRU policy update.
            }
            break;
        }
        if (!currentSet.blocks[i].valid) {
            evictionIndexCur= i;
            break;
        }
        // For LRU eviction
        if (lruEviction && currentSet.blocks[i].accessedCounter < currentSet.blocks[currentSet.evictionIndex].accessedCounter) {
            currentSet.evictionIndex = i;
        }
        // For FIFO eviction
        if (fifoEviction && currentSet.blocks[i].loadCounter < currentSet.blocks[currentSet.evictionIndex].loadCounter) {
            currentSet.evictionIndex = i;
        }
        evictionIndexCur = currentSet.evictionIndex;
    }

    // If miss, we might need to evict and access main memory.
    if (!hit) {
        replaceBlock(evictionIndexCur, currentSet,command, bytesInBlock, tag);
    }
    totalCycles++;
    currentCycle++;  // Simulating time progression.
}

// Function to handle a memory write request.
void Cache::write(unsigned int address, int bytesInBlock) {
    totalStores++;
    char command = 'w';

    unsigned int index = (address / bytesInBlock) % numSets;
    unsigned int tag = address / (bytesInBlock * numSets);

    Set& currentSet = sets[index];

    bool hit = false;
    unsigned int evictionIndexCur = 0;

    // Check each block in the set for a hit or identify the block.
    for (int i = 0; i < blocksPerSet; ++i) {
        
        if (currentSet.blocks[i].valid && currentSet.blocks[i].tag == tag) {
            hit = true;
            storeHits++;
            
            if(!writeBack) {
                totalCycles += MEMORY_ACCESS_CYCLES;
            }
            else {
                currentSet.blocks[i].dirty = true;
            }

            if (lruEviction) {
                currentSet.blocks[i].accessedCounter = currentCycle;
            }
            break;
        }
        if (!currentSet.blocks[i].valid) {
            evictionIndexCur = i;
            break;
        }

        // For LRU eviction
        if (lruEviction && currentSet.blocks[i].accessedCounter < currentSet.blocks[currentSet.evictionIndex].accessedCounter) {
            currentSet.evictionIndex = i;
        }

        // For FIFO eviction
        if (fifoEviction && currentSet.blocks[i].loadCounter < currentSet.blocks[currentSet.evictionIndex].loadCounter) {
            currentSet.evictionIndex = i;
        }
        evictionIndexCur = currentSet.evictionIndex;
    }

    // If miss, similar steps as in read but more nuances based on write policies.
    if (!hit) {
        if(writeAllocate) {
            replaceBlock(evictionIndexCur, currentSet, command, bytesInBlock, tag);

            if (!writeBack) {
                totalCycles += MEMORY_ACCESS_CYCLES;
            }
        }
        // not write allocate, so update it simple
        else {
            storeMisses++;
            totalCycles += (bytesInBlock / 4 + 1) * MEMORY_ACCESS_CYCLES;
        }
    }
    totalCycles++;
    currentCycle++;
}

void Cache::replaceBlock(unsigned int evictionIndex, Set& currentSet, char command, int bytesInBlock, unsigned int tag) {
    //command is write/store
    if (command =='w') {
            storeMisses++;
            Block& evictedBlock = currentSet.blocks[evictionIndex];
            //evictedBlock.dirty = writeBack;
            if (evictedBlock.valid && evictedBlock.dirty) {
                totalCycles += (bytesInBlock / 4) * MEMORY_ACCESS_CYCLES;
                evictedBlock.dirty = false;
            }
            evictedBlock.dirty = writeBack;
            totalCycles += bytesInBlock/4 * MEMORY_ACCESS_CYCLES;
//make sure that this is write, is write back and write through done correctly??
            evictedBlock.valid = true;
            evictedBlock.tag = tag;
            evictedBlock.accessedCounter = currentCycle;

            if (fifoEviction) {
                currentSet.blocks[evictionIndex].loadCounter = currentCycle;
            }
    }
    //command is read/load
    else{
        loadMisses++;
        totalCycles += (bytesInBlock / 4) * MEMORY_ACCESS_CYCLES;  // Penalty for main memory access.

        Block& evictedBlock = currentSet.blocks[evictionIndex];

        // If block is dirty, write back to main memory.
        if (evictedBlock.valid && evictedBlock.dirty) {
            totalCycles += (bytesInBlock / 4) * MEMORY_ACCESS_CYCLES;
            evictedBlock.dirty = false;
        }

        // Update block with new data.
        evictedBlock.valid = true;
        evictedBlock.tag = tag;
        evictedBlock.accessedCounter = currentCycle;  // Set block as recently accessed.

        if (fifoEviction) {
            currentSet.blocks[evictionIndex].loadCounter = currentCycle;
        }
    }
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