#ifndef PAGE_MAP_INDEXER_H
#define PAGE_MAP_INDEXER_H

#include <stdint.h>

class PageMapIndexer {
public:
    PageMapIndexer(uint64_t virtualAddress);
    uint64_t PDP_i;     // Page Directory Pointer Index
    uint64_t PD_i;      // Page Directory Index
    uint64_t PT_i;      // Page Table Index
    uint64_t P_i;       // Page Index
};


#endif
