#include "kernelUtil.h"
#include "memory/heap.h"
#include "scheduling/pit/pit.h"

uint8_t testbuffer[20];
extern "C" void _start(BootInfo* bootInfo) {
    KernelInfo kernelInfo = InitializeKernel(bootInfo);
    PageTableManager* pageTableManager = kernelInfo.pageTableManager;

    PIT::SetDivisor(65535);

    
    GlobalRenderer->Print("Im in the new Page Map!");

    GlobalRenderer->Next();
    GlobalRenderer->Print(to_hstring((uint64_t) bootInfo->rsdp));

    GlobalRenderer->Next();
    GlobalRenderer->Print(to_hstring((uint64_t) malloc(0x8000)));
    GlobalRenderer->Next();
    void* address =  malloc(0x8000);
    GlobalRenderer->Print(to_hstring((uint64_t) address));
    GlobalRenderer->Next();
    GlobalRenderer->Print(to_hstring((uint64_t) malloc(0x100)));
    GlobalRenderer->Next();

    free(address);
    GlobalRenderer->Print(to_hstring((uint64_t) malloc(0x100)));
    GlobalRenderer->Next();

    for (int t = 0; t < 200; t++) {
        GlobalRenderer->Print("g");
        PIT::Sleep(10);
    }


    /* asm ("int $0x0e"); */

    /* int* test = (int*)0x80000000000; */
    /* *test = 2; */

    while(true);



    /*
    PageMapIndexer pageIndexer = PageMapIndexer(0x1000 * 52 + 0x50000 * 7);

    newRenderer.Print(to_string(pageIndexer.P_i));
    newRenderer.Print(" - ");
    newRenderer.Print(to_string(pageIndexer.PT_i));
    newRenderer.Print(" - ");
    newRenderer.Print(to_string(pageIndexer.PD_i));
    newRenderer.Print(" - ");
    newRenderer.Print(to_string(pageIndexer.PDP_i));
    newRenderer.Print(" - ");
    */

    /*

    newRenderer.CursorPosition = {0, newRenderer.CursorPosition.Y + 16};
    newRenderer.Print("Free RAM: ");
    newRenderer.Print(to_string(newAllocator.GetFreeRAM()));
    newRenderer.Print(" kb");
    
    newRenderer.CursorPosition = {0, newRenderer.CursorPosition.Y + 16};
    newRenderer.Print("Used RAM: ");
    newRenderer.Print(to_string(newAllocator.GetUsedRAM()));
    newRenderer.Print(" kb");

    newRenderer.CursorPosition = {0, newRenderer.CursorPosition.Y + 16};
    newRenderer.Print("Reserved RAM: ");
    newRenderer.Print(to_string(newAllocator.GetReservedRAM()));
    newRenderer.Print(" kb");


    for (int t = 0; t < 20; t++) {
        void* address = newAllocator.RequestPage();
        newRenderer.Print(to_hstring((uint64_t) address));
        newRenderer.CursorPosition = {0, newRenderer.CursorPosition.Y + 16};
    }
*/


    /*
    newRenderer.Print(to_string(GetMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mMapDescSize))); // total system memory

    Bitmap testBitmap;
    testBitmap.Buffer =  &testbuffer[0];
    testBitmap.Set(0, false);
    testBitmap.Set(1, true);
    testBitmap.Set(2, false);
    testBitmap.Set(3, false);
    testBitmap.Set(4, true);


    for (int i = 0; i < 20; i++) {
        newRenderer.CursorPosition = {0, newRenderer.CursorPosition.Y + 16};
        newRenderer.Print(testBitmap[i] ? "true" : "false");
    }
    */


            /*
    for (int i = 0; i < mMapEntries; i++) {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)bootInfo->mMap + (i * bootInfo->mMapDescSize));
        newRenderer.CursorPosition = {0, newRenderer.CursorPosition.Y + 16};
        newRenderer.Print(EFI_MEMORY_TYPE_STRINGS[desc->type]);
        newRenderer.Color = 0xffff00ff;
        newRenderer.Print(" ");
        newRenderer.Print(to_string(desc->numPages * 4096 / 1024));
        newRenderer.Print("kb");
        newRenderer.Color = 0xffffffff;
    }
    */


    /*
    newRenderer.Print("Test C++ Class");
    newRenderer.Print(to_string((uint64_t) 12345678));
    newRenderer.CursorPosition = {0, 16};
    newRenderer.Print(to_string((int64_t) -12345678));
    newRenderer.CursorPosition = {0, 32};
    newRenderer.Print(to_string((double) 14.32));
    newRenderer.CursorPosition = {0, 48};
    newRenderer.Print(to_hstring((uint64_t) 0x1234));
    newRenderer.CursorPosition = {0, 64};
    newRenderer.Print(to_hstring((uint32_t) 0x1234));
    newRenderer.CursorPosition = {0, 80};
    newRenderer.Print(to_hstring((uint16_t) 0x1234));
    newRenderer.CursorPosition = {0, 96};
    newRenderer.Print(to_hstring((uint8_t) 0x1234));
    */

    /*
    unsigned int y = 50;
    unsigned int BBP = 4; //Bytes per Pixel
    for (unsigned int x = 0; x < framebuffer->Width / 2 * BBP; x += BBP) {
        *(unsigned int*)(x + (y * framebuffer->PixelsPerScanLine * BBP) + framebuffer->BaseAddress) = 0xffffffff;
    }

    putChar(framebuffer, psf1_font, 0xffffffff, 'G', 10, 10);

    CursorPosition.X = 50;
    CursorPosition.Y = 120;
    for (int t = 0; t < 50; t += 1) {
    Print(framebuffer, psf1_font, 0xffffffff, "Hello kernel");
    }
    */
}
