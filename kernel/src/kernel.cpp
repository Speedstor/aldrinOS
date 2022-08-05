#include "sys/boot/kernelUtil.h"
#include "sys/memory/heap.h"
#include "sys/time/pit.h"

uint8_t testbuffer[20];
extern "C" void _start(BootInfo* bootInfo) {
    KernelInfo kernelInfo = InitializeKernel(bootInfo);
    PageTableManager* pageTableManager = kernelInfo.pageTableManager;

    while(true) {
        asm ("hlt");
    }
}
