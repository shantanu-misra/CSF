// Important: do not add any additional #include directives!
// The only C library functions that may be used are
//
// - fgetc
// - malloc
// - free

#include <stdlib.h>
#include "wcfuncs.h"

// Compute a hash code for the given NUL-terminated
// character string.
//
// The hash algorithm should be implemented as follows:
//
// uint32_t hash_code = 5381
// for each character c of w in order {
//   hash_code = hash_code * 33 + c
// }
//
// Note that the character values should be treated as
// being unsigned (in the range 0..255)
uint32_t wc_hash(const unsigned char *w) {
  uint32_t hash_code = 5381;
  const unsigned char *ptr = w;

  // loop that will go on until it encounters a null character in the string
  while (*ptr) {
    // update the hash code according to the algorithm
    hash_code = hash_code * 33 + *ptr;

    // move to the next character in the string
    ptr++;
  }

  return hash_code;
}


// Compare two strings lexicographically. Return
//
// - a negative value if lhs string is less than rhs string
// - 0 if lhs string is identical to rhs string
// - a positive value if lhs string is greater than rhs string
//
// Lexicographical comparison is a generalization of alphabetical
// order, but using character codes. If one string is a prefix
// of the other, it is considered as "less than". E.g.,
// "hi" would compare as less than "high".
int wc_str_compare(const unsigned char *lhs, const unsigned char *rhs) {

  // iterate through each character of both strings until
  // either the left sting ends or first unequal pair of characters is found
  while (*lhs && (*lhs == *rhs)) {
    lhs++;
    rhs++;
  }

  // return the difference between the ASCII characters
  return *(const unsigned char *)lhs - *(const unsigned char *)rhs;
}

// Copy NUL-terminated source string to the destination buffer.
void wc_str_copy(unsigned char *dest, const unsigned char *source) {
  int  i = 0;
  // iterate through source string
  for (i = 0; source[i] != '\0'; i++) {
    // copy the current character from the source string to the destination string
    dest[i] = source[i];
  }
  // null terminate the destination string
  dest[i] = '\0';
}

// Return 1 if the character code in c is a whitespace character,
// false otherwise.
//
// For the purposes of this function, a whitespace character is one of
//
//   ' '
//   '\t'
//   '\r'
//   '\n'
//   '\f'
//   '\v'
int wc_isspace(unsigned char c) {
  if (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f' || c == '\v') {
    return 1;
  } 
  else {
    return 0;
  }
}

// Return 1 if the character code in c is an alphabetic character
// ('A' through 'Z' or 'a' through 'z'), 0 otherwise.
int wc_isalpha(unsigned char c) {
  if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
    return 1;
  } else {
    return 0;
  }
}

// Read the next word from given input stream, storing
// the word in the array pointed-to by w. (This array should be
// assumed to be MAX_WORDLEN+1 elements in size.) Return 1
// if a word is read successfully, 0 otherwise.
//
// For the purposes of this function, a word is a sequence of
// 1 or more non-whitespace characters.
//
// If a sequence of non-whitespace characters has more than
// MAX_WORDLEN characters, then only the first MAX_WORDLEN
// characters in the sequence should be stored in the array.
int wc_readnext(FILE *in, unsigned char *w) {
  int i = 0;  // Index for the character being read
  int c;      // Variable to hold the read character

  // Skip leading whitespace characters
  do {
    c = fgetc(in);
  } while (c != EOF && wc_isspace((unsigned char)c));

  // If we encountered EOF or an error before finding a word, return 0
  if (c == EOF) {
    return 0;
  }

  // Read non-whitespace characters up to MAX_WORDLEN
  while (c != EOF && !wc_isspace((unsigned char)c)) {
    // Check if we have reached the maximum word length
    if (i < MAX_WORDLEN) {
      w[i++] = (unsigned char)c;
    }
    // Read the next character
    c = fgetc(in);
  }

  // Null-terminate the word
  w[i] = '\0';

  // Return 1 to indicate that a word was successfully read
  return 1;
}

// Convert the NUL-terminated character string in the array
// pointed-to by w so that every letter is lower-case.
void wc_tolower(unsigned char *w) {
  // start a loop that will go on until it encounters a null character in the string
  for (int i = 0; w[i] != '\0'; i++) {
    // check if the current character is an uppercase alphabetic character
    if (w[i] >= 'A' && w[i] <= 'Z') {
      // If it is, convert it to lowercase by adding the difference between 'A' and 'a' to it
      w[i] = w[i] + ('a' - 'A');
    }
  }
}

// Remove any non-alphaabetic characters from the end of the
// NUL-terminated character string pointed-to by w.
void wc_trim_non_alpha(unsigned char *w) {
  int length = 0;  // Variable to store the length of the string

  // Calculate the length of the string
  while (w[length] != '\0') {
    length++;
  }

  // Start from the end of the string and move towards the beginning
  // Remove any non-alpha characters from the end of the string
  while (length > 0 && !wc_isalpha(w[length - 1])) {
    length--;
  }

  // Null-terminate the trimmed string
  w[length] = '\0';
}

// Search the specified linked list of WordEntry objects for an object
// containing the specified string.
//
// If a matching object is found, set the int variable pointed-to by
// inserted to 0 and return a pointer to the matching object.
//
// If a matching object is not found, allocate a new WordEntry object,
// set its next pointer to point to what head points to (i.e., so the
// new object is at the head of the list), set the variable pointed-to
// by inserted to 1, and return a pointer to the new node. Note that
// the new node should have its count value set to 0. (It is the caller's
// job to update the count.)
// struct WordEntry *wc_find_or_insert(struct WordEntry *head, const unsigned char *s, int *inserted) {
//   // Initialize a pointer to traverse the linked list
//   struct WordEntry* current = head;

//   // Iterate through the linked list to find the matching entry or the appropriate insertion point
//   while (current != NULL) {
//     int cmp = wc_str_compare(s, current->word);

//     // If a matching entry is found, return it
//     if (cmp == 0) {
//       *inserted = 0; // Entry already exists
//       return current;
//     }
//     current = current->next;
//   }

//   // If we reach here, the entry does not exist, and we need to insert it
//   struct WordEntry *newEntry = (struct WordEntry *)calloc(1, sizeof(struct WordEntry));
//   if (newEntry == NULL) {
//     *inserted = -1; // Memory allocation failed
//     return NULL;
//   }

//   // Initialize the new entry
//   wc_str_copy(newEntry->word, s);
//   newEntry->count = 0;
//   newEntry->next = NULL;

//   // Insert New Node
//   newEntry->next = head;
//   head = newEntry;

//   *inserted = 1;
//   return newEntry; // Return the pointer to the new node
// }
struct WordEntry *wc_find_or_insert(struct WordEntry *head, const unsigned char *s, int *inserted) {
  struct WordEntry* current = head;

  // Search for the word in the linked list
  while (current != NULL) {
    int cmp = wc_str_compare(s, current->word);
    if (cmp == 0) {
      *inserted = 0; 
      return current;  // Word found, return the existing node
    }
    current = current->next;
  }

  // If word not found, create a new node and insert it at the beginning
  struct WordEntry *newEntry = (struct WordEntry *)malloc(sizeof(struct WordEntry));
  if (!newEntry) {
    *inserted = -1;
    return NULL;
  }
  wc_str_copy(newEntry->word, s);
  newEntry->count = 0;
  newEntry->next = head;  // Point new entry to the current head

  *inserted = 1;
  return newEntry; // Return the newly inserted node
}


// Find or insert the WordEntry object for the given string (s), returning
// a pointer to it. The head of the linked list which contains (or should
// contain) the entry for s is the element of buckets whose index is the
// hash code of s mod num_buckets.
//
// Returns a pointer to the WordEntry object in the appropriate linked list
// which represents s.
struct WordEntry *wc_dict_find_or_insert(struct WordEntry *buckets[], unsigned num_buckets, const unsigned char *s) {
  // Calculate the hash code for the string
  uint32_t hash = wc_hash(s);

  // Determine the bucket index by taking the hash modulo the number of buckets
  unsigned index = hash % num_buckets;

  // Search the linked list in the selected bucket for a matching entry
  int inserted = 0; // This will be updated by wc_find_or_insert
  struct WordEntry *entry = wc_find_or_insert(buckets[index], s, &inserted);

  if (inserted == 1) {
    buckets[index] = entry; // Update head of the bucket's linked list
  }

  // Return the pointer to the WordEntry object in the appropriate linked list
  return entry;
}

// Free all of the nodes in given linked list of WordEntry objects.
void wc_free_chain(struct WordEntry *p) {
  // start a loop that will go on until it encounters a null pointer in the linked list
  while (p != NULL) {
    // save the pointer to the next node before freeing the current node
    struct WordEntry *nextNode = p->next;

    // free the current node
    free(p);

    // move to the next node
    p = nextNode;
  }
}