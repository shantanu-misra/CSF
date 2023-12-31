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
  UInt256 result = {0};
  int len = strlen(hex);
  int shift = 0;
  int i = len - 1;

  // Loop through the input string in reverse order and convert hexadecimal digits to binary
  while (i >= 0 && shift < 256) {
    char c = hex[i];
    uint32_t value = 0;

    // Check if the character is a valid hexadecimal digit (0-9, a-f, A-F)
    if (c >= '0' && c <= '9') {
      value = c - '0';
    } else if (c >= 'a' && c <= 'f') {
      value = c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
      value = c - 'A' + 10;
    }

    // Update the result's data array with the converted value at the appropriate bit position
    result.data[shift / 32] |= value << (shift % 32);
    shift += 4;
    i--;
  }
  return result;
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val) {

  char* hex = (char*) malloc(64+1);
  char* buff = (char*) malloc(8+1);

  int first_sig_index = 0;
  int h_idx = 0;

  // Find the index of the most significant non-zero word in val.data
  for (int i = 7; i > -1; i--) {
    if ( (i != 0) && val.data[i] == (uint32_t)0) {
      continue;
    }
    first_sig_index = i;
    break;
  }

  // Convert the most significant non-zero word to hexadecimal and store it in buff
  sprintf(buff, "%x", val.data[first_sig_index]);

  // Copy the characters from buff to the output hex string
  for (int i = 0; buff[i] != '\0'; i++) {
    hex[h_idx++] = buff[i];
  }

  // Convert and copy the remaining words to the output hex string
  for (int i = first_sig_index - 1; i > -1; i--) {
    sprintf(buff, "%08x", val.data[i]); // Convert the word to an 8-character hexadecimal string
    for (int j = 0; j < 8; j++) {
      hex[h_idx++] = buff[j]; // Copy each character to the output hex string
    }
  }

  hex[h_idx] = '\0';
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

  // char* bin_str = (char*) malloc(256 + 1);
  char* bin_str = (char*) calloc(256+1, sizeof(char));

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
  bin_str[256] = '\0';
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

// Helper function to remove leading 0s in hex string
char* remove_leading_zeros(char* hex) {
  // Find the first non-zero character
  int nonZeroPos = 0;
  while (hex[nonZeroPos] == '0' && hex[nonZeroPos + 1] != '\0') {
    nonZeroPos++;}

  // Create a new string starting from the non-zero position
  char* result = strdup(hex + nonZeroPos);

  return result;
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
  full_hex[64] = '\0';
  for (int i = 0; i < 64; i++) {
    full_hex[i] = '0';
  }

  // convert UInt256 val into a hex
  char* actual_hex = uint256_format_as_hex(val);
  int actual_hex_length = strlen(actual_hex);

  // transfer the actual hex into the full hex with leading 0s
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

  char* final_hex = remove_leading_zeros(shifted_hex); // remove leading 0s from hex

  // convert hex to UInt256
  result = uint256_create_from_hex(final_hex);

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
  full_hex[64] = '\0';
  for (int i = 0; i < 64; i++) {
    full_hex[i] = '0';
  }

  // convert UInt256 val into a hex
  char* actual_hex = uint256_format_as_hex(val);
  int actual_hex_length = strlen(actual_hex);

  // initialise hex string with all zeros
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
  memmove(bin + nbits, bin, len - nbits); // Shift the remaining characters to the right
  strncpy(bin, temp, nbits); // Copy the temporary buffer to the beginning

  // convert binary back to hex
  char* shifted_hex = convert_bin_str_to_hex_str(bin);

  char* final_hex = remove_leading_zeros(shifted_hex); // remove leading 0s

  // convert hex to UInt256
  result = uint256_create_from_hex(final_hex);

  return result;
}
