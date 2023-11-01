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
So, we will keep the total cache size constant at 32KB. We chose this value because its resonably big, but not too big.

The top-10 best caches (minimum total-cycles) are: 
num_sets    num_blocks  bytes_in_blocks
1	        1024	    4	                write-allocate	write-back	lru
2	        512	        4	                write-allocate	write-back	lru
8	        128	        4	                write-allocate	write-back	lru
4	        256	        4	                write-allocate	write-back	lru
32	        32	        4	                write-allocate	write-back	lru
64	        16	        4	                write-allocate	write-back	lru
16	        64	        4	                write-allocate	write-back	lru
128	        8	        4	                write-allocate	write-back	lru
256	        4	        4	                write-allocate	write-back	lru

(this table produced by `analysis.cpp` was sorted by `total_cycles` in excel and pasted back into the file submitted via gradescope)

According to this, the best, ideal cache is a fully associative cache with 1024 blocks with 4 bytes in a block.
This makes sense since fully-associative caches reduce cache collions. However, in real world they are very complicated to implement.

The distribution of the cache configurations in the list suggests that a cache with a larger number of sets and a smaller number 
of blocks per set tends to perform better. This is because a larger number of sets allows for more flexibility in mapping memory addresses 
to cache blocks, while a smaller number of blocks per set reduces the likelihood of conflicts between memory addresses that map to the same set.

All of the caches in the list use a write-allocate policy and a write-back strategy, which areconsidered to be more efficient 
than other policies and strategies because in write-allocate when a write miss occurs in the cache, the block of data is 
loaded into the cache, then the write is performed. This can be beneficial because it allows subsequent writes to the 
same block to hit in the cache. LRU being better than FIFO is more obvious. If the cache is using a block more frequently, 
then its beneficifal to keep it in the cache rather than remove it.

Because we're multiplying by `bytes_in_block` when calculating total_cycles, increasing `bytes_in_block` makes `tottal_cycles`
increase pretty quickly. 

If we were to choose one of these caches to use in real life, we would pick the 10th one. This is because its a 4-way set associative cache. 
These have a comparably high hit-rate and also are much, much simpler to implement compared to fully associative caches, 
where data can be stored anywhere. It's also cost effective in the sense that it strikes a good balance between cost and performance.


# PROJECT CONTRIBUTIONS
Shantanu: main, read, and write methods, cache analysis
Sebastian: replace block method and implamentation of write through,back