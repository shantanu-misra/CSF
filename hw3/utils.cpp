#include "utils.h"
#include <sstream>
#include <iostream>



// Convert a hexadecimal string to an unsigned integer
unsigned int hexToUInt(const std::string& hexStr) {
    unsigned int num;
    std::stringstream ss;
    ss << std::hex << hexStr;
    ss >> num;
    return num;
}

// Generate a mask with 'numBits' set to 1 from the rightmost side
unsigned int generateBitMask(unsigned int numBits) {
    if(numBits >= sizeof(unsigned int) * 8) {
        return ~0; // All bits set
    }
    return (1 << numBits) - 1;
}

// Extract the set index from the address
unsigned int extractSetIndex(unsigned int address, unsigned int indexBits, unsigned int blockOffsetBits) {
    // Assuming that the address is divided into Tag | Index | Block Offset
    unsigned int mask = generateBitMask(indexBits);
    return (address >> blockOffsetBits) & mask;
}

// Extract the tag from the address
unsigned int extractTag(unsigned int address, unsigned int indexBits, unsigned int blockOffsetBits) {
    // Extracts tag by shifting right by (indexBits + blockOffsetBits)
    return address >> (indexBits + blockOffsetBits);
}
