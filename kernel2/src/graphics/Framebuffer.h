#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stddef.h>

struct Framebuffer {
    void* BaseAddress;
    size_t BufferSize;
    unsigned int Width;
    unsigned int Height;
    unsigned int PixelsPerScanLine;
};

#endif

