#ifndef KERNEL_UTIL_H
#define KERNEL_UTIL_H

#include <stdint.h>
#include "../../graphics/BasicRenderer.h"
#include "../../inc/cstr.h"
#include "efiMemory.h"
#include "../memory/memory.h"
#include "../../inc/Bitmap.h"
#include "../memory/paging/PageFrameAllocator.h"
#include "../memory/paging/PageMapIndexer.h"
#include "../memory/paging/paging.h"
#include "../memory/paging/PageTableManager.h"
#include "../../userinput/mouse.h"
#include "../devices/acpi.h"
#include "../devices/pci.h"

struct BootInfo {
    Framebuffer* framebuffer;
    PSF1_FONT* psf1_Font;
    EFI_MEMORY_DESCRIPTOR* mMap;
    uint64_t mMapSize;
    uint64_t mMapDescSize;
    ACPI::RSDP2* rsdp;
};

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

struct KernelInfo {
    PageTableManager* pageTableManager;
};

KernelInfo InitializeKernel(BootInfo* bootInfo);


#endif
