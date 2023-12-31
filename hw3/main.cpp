#include "cache.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> // for std::strtol

// Utility function to check if a number is a power of 2
bool isPowerOfTwo(int n) {
    return (n && !(n & (n - 1)));
}

int main(int argc, char* argv[]) {
    if(argc != 7) {
        std::cerr << "Usage: " << argv[0] << " <num_sets> <num_blocks> <bytes_in_block> <write_policy> <write_through_or_back> <eviction_policy> <trace_file>" << std::endl;
        return 1;
    }

    // Extract and process the command-line arguments
    int numSets = std::stoi(argv[1]);
    int blocksPerSet = std::stoi(argv[2]);
    int bytesInBlock = std::stoi(argv[3]);

    bool writeAllocate = (std::string(argv[4]) == "write-allocate");
    bool writeBack = (std::string(argv[5]) == "write-back");
    bool lruEviction = (std::string(argv[6]) == "lru");
    bool fifoEviction = (std::string(argv[6]) == "fifo");

    // Check for configuration validity
    if (!isPowerOfTwo(bytesInBlock) || !isPowerOfTwo(numSets)) {
        std::cerr << "Error: Block size and number of sets must be a power of 2." << std::endl;
        return 1;
    }
    if (bytesInBlock < 4) {
        std::cerr << "Error: Block size must be at least 4." << std::endl;
        return 1;
    }
    if (writeBack && !writeAllocate) {
        std::cerr << "Error: Cannot use write-back without write-allocate." << std::endl;
        return 1;
    }
    if (!lruEviction && !fifoEviction) {
        std::cerr << "Error: Replacement algorithim neither LRU nor FIFO." << std::endl;
        return 1;
    }

    Cache myCache(numSets, blocksPerSet, writeAllocate, writeBack, lruEviction, fifoEviction);
    std::string operation, address;
    std::string discard; // the last cahracter which will be discarded
    unsigned int addr;
    
    while(std::cin >> operation >> address >> discard) {
        // Convert hex string to unsigned int
        addr = std::strtol(address.c_str(), nullptr, 16);

        // Process the memory operation based on the operation type
        if(operation == "l") {
            myCache.read(addr, bytesInBlock);
        } else if(operation == "s") {
            myCache.write(addr, bytesInBlock);
        }
    }
    
    myCache.printSummary();

    return 0;
}
