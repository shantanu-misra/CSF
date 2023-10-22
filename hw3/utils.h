#ifndef UTILS_H
#define UTILS_H

#include <string>

// Convert a hexadecimal string to an unsigned integer
unsigned int hexToUInt(const std::string& hexStr);

// Generate a mask with 'numBits' set to 1 from the rightmost side
unsigned int generateBitMask(unsigned int numBits);

// Extract the set index from the address
unsigned int extractSetIndex(unsigned int address, unsigned int indexBits, unsigned int blockOffsetBits);

// Extract the tag from the address
unsigned int extractTag(unsigned int address, unsigned int indexBits, unsigned int blockOffsetBits);


#endif // UTILS_H
