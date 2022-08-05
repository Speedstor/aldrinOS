#include "kernelUtil.h"
#include "memory/heap.h"
#include "scheduling/pit/pit.h"

uint8_t testbuffer[20];
extern "C" void _start(BootInfo* bootInfo) {
    KernelInfo kernelInfo = InitializeKernel(bootInfo);
    PageTableManager* pageTableManager = kernelInfo.pageTableManager;

    while(true) {
        asm ("hlt");
    }
}
