#ifndef PAGE_TABLE_MANAGER_H
#define PAGE_TABLE_MANAGER_H

#include "paging.h"

class PageTableManager {
    public:
        PageTableManager(PageTable* PML4Address);
        PageTable* PML4;
        void MapMemory(void* virtualMemory, void* physicalMemory);
};

extern PageTableManager g_PageTableManager;

#endif
