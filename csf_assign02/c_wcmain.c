#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "wcfuncs.h"

#define HASHTABLE_SIZE 13249

int main(int argc, char **argv) {
    uint32_t total_words = 0;
    uint32_t unique_words = 0;
    const unsigned char *best_word = (const unsigned char *) "";
    uint32_t best_word_count = 0;

    struct WordEntry** buckets = (struct WordEntry**)malloc(HASHTABLE_SIZE * sizeof(struct WordEntry*));
    for (int i = 0; i < HASHTABLE_SIZE; i++) {
        buckets[i] = NULL;  // Initialize all bucket pointers to NULL
    }

    FILE *in;

    if (argc == 1) {
        // Read from stdin if no arguments provided
        in = stdin;
    } 
    else if (argc == 2) {
        in = fopen(argv[1], "r");
        if (in == NULL) {
            fprintf(stderr, "Error: Could not open file %s\n", argv[1]);
            free(buckets);
            return 1;
        }
    } 
    else {
        fprintf(stderr, "Usage: %s [input_file]\n", argv[0]);
        free(buckets);
        return 1;
    }

    unsigned char word[MAX_WORDLEN + 1];
    while (wc_readnext(in, word)) {
        total_words++;

        wc_tolower(word);
        wc_trim_non_alpha(word);

        struct WordEntry *entry = wc_dict_find_or_insert(buckets, HASHTABLE_SIZE, word);
        if (entry->count == 0) {
            unique_words++;
        }
        entry->count++;

        if (entry->count > best_word_count || (entry->count == best_word_count && wc_str_compare(word, best_word) < 0)) {
            best_word = entry->word;
            best_word_count = entry->count;
        }
    }

    fclose(in);

    printf("Total words read: %u\n", (unsigned int)total_words);
    printf("Unique words read: %u\n", (unsigned int)unique_words);
    printf("Most frequent word: %s (%u)\n", (const char *)best_word, best_word_count);

    for (int i = 0; i < HASHTABLE_SIZE; i++) {
        wc_free_chain(buckets[i]);
    }
    free(buckets);

    return 0;
}
