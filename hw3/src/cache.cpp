#include "cache.h"
#include "utils.h"
#include <iostream>

Cache::Cache() : cacheBlock("") {}

void Cache::processAddress(const std::string& address) {
    if(cacheBlock == address) {
        std::cout << "Cache Hit for address: " << address << std::endl;
    } else {
        if(cacheBlock == "") {
            std::cout << "Cache Miss and Load for address: " << address << std::endl;
        } else {
            std::cout << "Cache Miss and Replacement for address: " << address << std::endl;
        }
        cacheBlock = address;
    }
}

void Cache::printDetails() {
    std::cout << "Final Cache Block: " << cacheBlock << std::endl;
}
