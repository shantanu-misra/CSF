# BEST CACHE ANALYSIS

To determine the best cache, we will use `gcc.trace` because it is from real a program.

There are multiple ways a cache can be configured. For this program, the configuration depends on:

- num_sets: Specifies how many sets the cache has. A larger number typically means a bigger cache, 
but the actual size would also depend on block size and associativity.

- num_blocks: Indicates the associativity of the cache. This tells you how many blocks or lines are in each set. For example:
    If <num_blocks> is 1, the cache is direct-mapped.
    If <num_blocks> is equal to <num_sets>, the cache is fully associative.
    Otherwise, it's set-associative. The number of blocks tells you how many lines are in each set.

- bytes_in_block: Specifies the size of each block or line in the cache. This, combined with the number of sets and blocks, 
gives the total cache size.

write_policy: indicates the overall strategy for handling writes.

write_through_or_back:  specifies the cache's write strategy:
    Write Through: Writes are immediately reflected in both the cache and main memory.
    Write Back: Writes are reflected in the cache first, and main memory is updated later when the block is evicted.

eviction_policy: Dictates the strategy used to decide which block to evict when the cache experiences a miss. For us that is:
    LRU: Least recently used
    FIFO: First in first out

For our analysis, to make meaningful comparisons between different configurations, we will hold certain parameters constant.
1. First we will keep the total cache size constant, by varying the associativity of the cache.
2. Second, we will vary `num_sets`, keeping `num_blocks` and `bytes_in_block` constant.
3. Third, we will vary `bytes_in_block` keeping `num_blocks` and `num_sets` constant.

For all tests involving set-associative caches, we will be test the efficiency of 2, 4, or 8 way associative caches. 
We will run each configuration of `num_sets`, `num_blocks`, and `bytes_in_block`with every possible permutation of 
`write_policy`, `write_through_or_back` and `eviction_policy`.

From what we learned in class, we can predict that a set-associative cache would outperform both a directly mapped and a fully
associative cache.

# PROJECT CONTRIBUTIONS
Shantanu: main, read, and write methods, best cache analysis
Sebastian: replace block method and implamentation of write through,back