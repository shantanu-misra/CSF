#ifndef CACHE_H
#define CACHE_H

#include <string>

class Cache {
public:
    Cache();
    void processAddress(const std::string& address);
    void printDetails();
private:
    // Assuming a simple direct-mapped cache for illustration
    std::string cacheBlock;  // Stores a single address for simplicity
};

#endif
