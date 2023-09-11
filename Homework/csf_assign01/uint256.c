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

  char *hex = (char*) malloc(64+1);
  char* buff = (char*) malloc(8+1);

  int first_sig_index = 0;
  int hex_idx = 0;

  for (int i = 7; i > -1; i--) {
    if ( (i != 0) && val.data[i] == (uint32_t)0) {
      continue;
    }
    first_sig_index = i;
    break;
  }

  sprintf(buff, "%x", val.data[first_sig_index]);

  for (int i = 0; buff[i] != '\0'; i++) {
    hex[hex_idx++] = buff[i];
  }

  for (int i = first_sig_index - 1; i > -1; i--) {
    sprintf(buff, "%08x", val.data[i]);
    for (int j = 0; j < 8; j++) {
      hex[hex_idx++] = buff[j];
    }
  }
  hex[hex_idx] = '\0';

  // free buf
  free(buff);

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

// Helper function to convert hex strings to binary strings
char* convert_hex_str_to_bin_str(char* hex) {
  int length = strlen(hex);
  char* bin_str = malloc(256 + 1);

  for (int i = 0; i < length; i++) {
    switch (hex[i]) {
      case '0': strcat(bin_str, "0000"); break;
      case '1': strcat(bin_str, "0001"); break;
      case '2': strcat(bin_str, "0010"); break;
      case '3': strcat(bin_str, "0011"); break;
      case '4': strcat(bin_str, "0100"); break;
      case '5': strcat(bin_str, "0101"); break;
      case '6': strcat(bin_str, "0110"); break;
      case '7': strcat(bin_str, "0111"); break;
      case '8': strcat(bin_str, "1000"); break;
      case '9': strcat(bin_str, "1001"); break;
      case 'A': case 'a': strcat(bin_str, "1010"); break;
      case 'B': case 'b': strcat(bin_str, "1011"); break;
      case 'C': case 'c': strcat(bin_str, "1100"); break;
      case 'D': case 'd': strcat(bin_str, "1101"); break;
      case 'E': case 'e': strcat(bin_str, "1110"); break;
      case 'F': case 'f': strcat(bin_str, "1111"); break;
      default: break; // Invalid hexadecimal character
    }
  }
  return bin_str;
}

// Helper function to convert binanry string to hex
char* convert_bin_str_to_hex_str(char* bin) {
  int bin_length = strlen(bin);

  char* hex = malloc(bin_length + 1);

  for (int i = 0; i < bin_length; i += 4) {
    char binaryGroup[5]; // +1 for null terminator
    strncpy(binaryGroup, bin + i, 4);
    binaryGroup[4] = '\0';
    
    int decimalValue = strtol(binaryGroup, NULL, 2); // Convert binary to decimal
    sprintf(hex + (i / 4), "%x", decimalValue); // Convert decimal to hexadecimal
  }
  return hex;
}


// Return the result of rotating every bit in val nbits to
// the left.  Any bits shifted past the most significant bit
// should be shifted back into the least significant bits.
UInt256 uint256_rotate_left(UInt256 val, unsigned nbits) {
  UInt256 result;

  // mod nbits by 256 so that shifting makes sense
  nbits %= 256;

  // initialise hex string with all zeros
  char* full_hex = (char*) malloc(64+1);
  for (int i = 0; i < 64; i++) {
    full_hex[i] = '0';
  }
  full_hex[64] = '\0';

  // convert UInt256 val into a hex
  char* actual_hex = uint256_format_as_hex(val);
  int actual_hex_length = strlen(actual_hex);

  for (int i = 0; i < actual_hex_length; i++) {
    full_hex[64-strlen(actual_hex)+i] = actual_hex[i];
  }

  // convert hex to binary
  char* bin = convert_hex_str_to_bin_str(full_hex);

  // shift the binary
  int len = strlen(bin);
  char temp[nbits + 1]; // Create a temporary buffer to store the first 'n' characters
  strncpy(temp, bin, nbits); // Copy the first 'n' characters to the temporary buffer
  temp[nbits] = '\0';
  memmove(bin, bin + nbits, len - nbits + 1); // Shift the remaining characters to the beginning
  strcat(bin, temp); // Append the temporary buffer at the end

  // convert binary back to hex
  char* shifted_hex = convert_bin_str_to_hex_str(bin);

  // convert hex to UInt256
  result = uint256_create_from_hex(shifted_hex);

  return result;
}

// Return the result of rotating every bit in val nbits to
// the right. Any bits shifted past the least significant bit
// should be shifted back into the most significant bits.
UInt256 uint256_rotate_right(UInt256 val, unsigned nbits) {
  UInt256 result;

  // mod nbits by 256 so that shifting makes sense
  nbits %= 256;

  // initialise hex string with all zeros
  char* full_hex = (char*) malloc(64+1);
  for (int i = 0; i < 64; i++) {
    full_hex[i] = '0';
  }
  full_hex[64] = '\0';

  // convert UInt256 val into a hex
  char* actual_hex = uint256_format_as_hex(val);
  int actual_hex_length = strlen(actual_hex);

  for (int i = 0; i < actual_hex_length; i++) {
    full_hex[64-strlen(actual_hex)+i] = actual_hex[i];
  }

  // convert hex to binary
  char* bin = convert_hex_str_to_bin_str(full_hex);

  // Shift the binary to the right
  int len = strlen(bin);
  char temp[nbits + 1]; // Create a temporary buffer to store the last 'n' characters
  strncpy(temp, bin + (len - nbits), nbits); // Copy the last 'n' characters to the temporary buffer
  temp[nbits] = '\0';
  memmove(bin + nbits, bin, len - nbits + 1); // Shift the remaining characters to the right
  strncpy(bin, temp, nbits); // Copy the temporary buffer to the beginning

  // convert binary back to hex
  char* shifted_hex = convert_bin_str_to_hex_str(bin);

  // convert hex to UInt256
  result = uint256_create_from_hex(shifted_hex);

  return result;
}
