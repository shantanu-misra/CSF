Name: Shantanu Misra (smisra8)

Submission Details: 
MS1 (8/30): Added support for uint256_create_from_u32, uint256_create, uint256_get_bits.
MS2 (10/11): Added support for rest of the other functions

Implementation Details:
I added THREE helper functions to the code:
    1. char* convert_hex_str_to_bin_str(char* hex);
    2. char* convert_bin_str_to_hex_str(char* bin);
    3. char* remove_leading_zeros(char* hex);
These were necessary because of how I was implementing left and right shifts.
I first converted UInt256 into a hex string, converted that hex string into a binary.
Performing order presering shifts on this binary string was very simple. Then I converted 
this binary string back into a hex string and then into a UInt256.