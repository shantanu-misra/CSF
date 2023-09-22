#include <stdio.h>
#include <stdint.h>
#include "wcfuncs.h"

// Suggested number of buckets for the hash table
#define HASHTABLE_SIZE 13249

// TODO: prototypes for helper functions

int main(int argc, char **argv) {
  // stats (to be printed at end)
  uint32_t total_words = 0;
  uint32_t unique_words = 0;
  const unsigned char *best_word = (const unsigned char *) "";
  uint32_t best_word_count = 0;

  // Create an array of WordEntry pointers for the hash table (buckets)
  struct WordEntry *buckets[HASHTABLE_SIZE] = { NULL };

  // Check for the correct number of command-line arguments
  if (argc != 2) {
    fprintf(stderr, "Usage: %s input_file\n", argv[0]);
    return 1;  // Return a non-zero exit code to indicate an error
  }

  // Open the input file for reading
  FILE *in = fopen(argv[1], "r");

  // Check if the file was opened successfully
  if (in == NULL) {
    fprintf(stderr, "Error: Could not open file %s\n", argv[1]);
    return 1;  // Return a non-zero exit code to indicate an error
  }

  // Buffer to store the word read from the input
  unsigned char word[MAX_WORDLEN + 1];

  // Read words from the input file and update word counts
  while (wc_readnext(in, word)) {
    total_words++;

    // Convert the word to lowercase and trim non-alphabetic characters
    wc_tolower(word);
    wc_trim_non_alpha(word);

    // Find or insert the word in the hash table
    struct WordEntry *entry = wc_dict_find_or_insert(buckets, HASHTABLE_SIZE, word);

    // Update the word count in the entry
    entry->count++;

    // Check if this is a new unique word
    if (entry->count == 1) {
      unique_words++;
    }

    // Check if this word has a higher count than the current best word
    if (entry->count > best_word_count || (entry->count == best_word_count && wc_str_compare(word, best_word) < 0)) {
      best_word = entry->word;
      best_word_count = entry->count;
    }
  }

  // Close the input file
  fclose(in);

  // TODO: Free allocated memory for the WordEntry objects

  // Print the statistics
  printf("Total words read: %u\n", (unsigned int)total_words);
  printf("Unique words read: %u\n", (unsigned int)unique_words);
  printf("Most frequent word: %s (%u)\n", (const char *)best_word, best_word_count);

  return 0;  // Return 0 to indicate success
}


// TODO: definitions of helper functions
