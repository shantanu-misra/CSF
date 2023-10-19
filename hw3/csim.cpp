#include <iostream>
#include <string>
#include <fstream>
#include <cmath>

int main(int argc, char* argv[]) {
    // File mode: 2 arguments (program name + trace file path)
    if(argc == 2) {
        // Parse command-line argument for the trace file
        std::string traceFilePath = argv[1];

        // Open and read memory traces from file
        std::ifstream traceFile(traceFilePath);
        if(!traceFile.is_open()) {
            std::cerr << "Error: Could not open trace file: " << traceFilePath << "\n";
            return 1;
        }

        std::string operation;
        std::string address;
        int value;

        while(traceFile >> operation >> address >> value) {
            // For now, just print the operation and address to confirm they were read correctly
            std::cout << "Operation: " << operation << ", Address: " << address << "\n";
        }

        traceFile.close();
    }
    // Command-line mode: 8 arguments (program name + 7 configuration parameters)
    else if(argc == 8) {
        // Parse command-line arguments
        int numSets = std::stoi(argv[1]);
        int numBlocks = std::stoi(argv[2]);
        int numBytes = std::stoi(argv[3]);
        std::string writeAllocatePolicy = argv[4];
        std::string writePolicy = argv[5];
        std::string evictionPolicy = argv[6];

        // Sample memory trace from the last argument
        std::string memoryTrace = argv[7];

        // Validate that numSets, numBlocks, and numBytes are positive powers-of-2
        if(numSets <= 0 || numBlocks <= 0 || numBytes < 4 || (numSets & (numSets - 1)) != 0 || 
           (numBlocks & (numBlocks - 1)) != 0 || (numBytes & (numBytes - 1)) != 0) {
            std::cerr << "Error: Cache design parameters must be positive powers-of-2.\n";
            return 1;
        }

        // Validate writeAllocatePolicy, writePolicy, and evictionPolicy
        if(writeAllocatePolicy != "write-allocate" && writeAllocatePolicy != "no-write-allocate") {
            std::cerr << "Error: Invalid write-allocate policy.\n";
            return 1;
        }
        if(writePolicy != "write-through" && writePolicy != "write-back") {
            std::cerr << "Error: Invalid write policy.\n";
            return 1;
        }
        if(evictionPolicy != "lru" && evictionPolicy != "fifo") {
            std::cerr << "Error: Invalid eviction policy.\n";
            return 1;
        }

        // Display the memory trace (for now, to verify the command-line mode works)
        std::cout << "Memory Trace: " << memoryTrace << "\n";
    }
    else {
        std::cerr << "Error: Invalid number of command-line arguments.\n";
        return 1;
    }

    return 0;
}
