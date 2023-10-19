#ifndef __CSIM_H__
#define __CSIM_H__

#include <cstdint>
#include <vector>
#include <map>

struct Slot {
    uint32_t tag;
    bool valid;
    bool dirty;
    uint32_t loadTimeStamp;
    uint32_t accessTimeStamp;
};

struct Set {
    std::vector<Slot> slots;
    std::map<uint32_t, Slot *> index;
};

struct Cache {
    std::vector<Set> sets;
    bool isWriteAllocate; // alternative is no write allocated
    bool isWriteThrough; // alteranative is write back
    bool isLru;  // alternative is fifo
};

void loadOperation();

void storeOperation();

#endif 


/*

You will design and implement a cache simulator that can be used to study and 
compare the effectiveness of various cache configurations. Your simulator will 
read a memory access trace from standard input, simulate what a cache based on 
certain parameters would do in response to these memory access patterns, and 
finally produce some summary statistics to standard output.

Note that you should assume that each load or store in the trace accesses at 
most 4 bytes of data, and that no load or store accesses data which spans 
multiple cache blocks (a.k.a. “lines”.)

Your simulation is only concerned with hits and misses, at no point do you need 
the actual data that’s stored in the cache; that’s the reason why the trace 
files do not contain that information in the first place.

Don’t try to implement all the options right away, start by writing a simulator 
that can only run direct-mapped caches with write-through and no-write-allocate. 
Once you have that working, extend step-by-step to make the other design parameters work. 

For part (b), you’ll use the memory traces as well as your simulator to determine which 
cache configuration has the best overall effectiveness. You should take a variety of 
properties into account: hit rates, miss penalties, total cache size (including overhead), 
etc. In your README.txt, describe in detail what experiments you ran (and why!), what 
results you got (and how!), and what, in your opinion, is the best cache configuration of them all
*/