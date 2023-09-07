#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uint256.h"

// Create a UInt256 value from a single uint32_t value.
// Only the least-significant 32 bits are initialized directly,
// all other bits are set to 0.
UInt256 uint256_create_from_u32(uint32_t val) {
  UInt256 result;

  // Initialize the least-significant 32 bits with the given value
  result.data[0] = val;

  // Set all other bits to 0
  for (int i = 1; i < 8; i++) {
    result.data[i] = 0;
  }

  return result;
}

// Create a UInt256 value from an array of NWORDS uint32_t values.
// The element at index 0 is the least significant, and the element
// at index 3 is the most significant.
UInt256 uint256_create(const uint32_t data[8]) {
  UInt256 result;

  for (int i = 0; i < 8; i++) {
    result.data[i] = data[i];
  }
  return result;
}

// Create a UInt256 value from a string of hexadecimal digits.
UInt256 uint256_create_from_hex(const char *hex) {
  UInt256 result;
  memset(result.data, 0, sizeof(result.data)); // initialise to 0
  size_t len = strlen(hex);

  // Start from the rightmost 8 hex digits and work left
  int d_idx = 0;
  int h_idex = len - 1;

  while (d_idx < 8 && h_idex >= 0) {
    char chnk_str[9]; // 8 hex digits + null terminator
    int chnk_len = 0;

    while (h_idex >= 0 && chnk_len < 8) {
      chnk_str[chnk_len] = hex[h_idex];
      h_idex--;
      chnk_len++;
    }

    chnk_str[chnk_len] = '\0';
    
    // Convert the chunk to uint32_t and assign it
    result.data[d_idx++] = (uint32_t) strtoul(chnk_str, NULL, 16);
  }
  return result;
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val) {
  char *hex = NULL;
  // TODO: implement
  return hex;
}

// Get 32 bits of data from a UInt256 value.
// Index 0 is the least significant 32 bits, index 3 is the most
// significant 32 bits.
uint32_t uint256_get_bits(UInt256 val, unsigned index) {
  uint32_t bits;
  
  bits = val.data[index];
  return bits;
}

// Compute the sum of two UInt256 values.
UInt256 uint256_add(UInt256 left, UInt256 right) {
  UInt256 sum;
  uint32_t carry = 0; // Initialize the carry to 0

  for (int i = 0; i < 8; i++) {
    // Calculate the sum of the current column, considering the carry
    uint64_t column_sum = (uint64_t)left.data[i] + right.data[i] + carry;

    // Extract the lower 32 bits as the result for the current column
    sum.data[i] = (uint32_t)column_sum;

    // Update the carry for the next column
    carry = (uint32_t)(column_sum >> 32);
  }

  return sum;
}


// Compute the difference of two UInt256 values.
UInt256 uint256_sub(UInt256 left, UInt256 right) {
  // Compute the two's complement negation of 'right'
  UInt256 neg_right = uint256_negate(right);

  // Add 'left' and the negated 'right' to get the difference
  return uint256_add(left, neg_right);
}

// Return the two's-complement negation of the given UInt256 value.
UInt256 uint256_negate(UInt256 val) {
  UInt256 result;
  uint32_t one = 1;

  // first negate all bits
  for (int i = 0; i < 8; i++) {
    result.data[i] = ~val.data[i];
  }

  // add one
  result = uint256_add(result, uint256_create_from_u32(one));

  return result;
}

// Return the result of rotating every bit in val nbits to
// the left.  Any bits shifted past the most significant bit
// should be shifted back into the least significant bits.
UInt256 uint256_rotate_left(UInt256 val, unsigned nbits) {
  UInt256 result;
  // TODO: implement
  return result;
}

// Return the result of rotating every bit in val nbits to
// the right. Any bits shifted past the least significant bit
// should be shifted back into the most significant bits.
UInt256 uint256_rotate_right(UInt256 val, unsigned nbits) {
  UInt256 result;
  // TODO: implement
  return result;
}
