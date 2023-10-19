#include "cache.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[]) {
    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <trace_file>" << std::endl;
        return 1;
    }

    std::string traceFilename = argv[1];
    std::ifstream traceFile(traceFilename);
    
    if(!traceFile.is_open()) {
        std::cerr << "Failed to open the trace file." << std::endl;
        return 1;
    }

    Cache myCache;
    std::string address;
    
    while(traceFile >> address) {
        myCache.processAddress(address);
    }

    traceFile.close();
    myCache.printDetails();

    return 0;
}
