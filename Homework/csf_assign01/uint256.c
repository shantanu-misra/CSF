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
      chunk_len++;
    }

    chnk_str[chnk_len] = '\0';
    
    // Convert the chunk to uint32_t and assign it
    result.data[data_index++] = (uint32_t) strtoul(chnk_str, NULL, 16);
  }
  return result;
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val) {
  char *hex = NULL;

    // Calculate the total number of hex digits needed
    int total_digits = 1;
    for (int i = 7; i >= 0; i--) {
      uint32_t temp = val.data[i];
      while (temp >>= 4) {
        total_digits++;
      }
    }

    // Allocate memory for the hex string
    hex = (char*) malloc(total_digits + 1);
    if (hex == NULL) {
      return NULL;
    }

    // Convert each uint32_t value to a hex string and concatenate them
    for (int i = 7; i >= 0; i--) {
      int num_digits = 1;
      while (val.data[i] >>= 4) {
        num_digits++;
      }
      sprintf(hex_ptr, "%0*x", num_digits, val.data[i]);
      hex_ptr += num_digits;
    }

    // end with null terminator
    *hex_ptr = '\0';

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
  // TODO: implement
  return sum;
}

// Compute the difference of two UInt256 values.
UInt256 uint256_sub(UInt256 left, UInt256 right) {
  UInt256 result;
  // TODO: implement
  return result;
}

// Return the two's-complement negation of the given UInt256 value.
UInt256 uint256_negate(UInt256 val) {
  UInt256 result;
  // TODO: implement
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
