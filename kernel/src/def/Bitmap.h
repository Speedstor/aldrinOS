#ifndef BITMAP_H
#define BITMAP_H

#include <stddef.h>
#include <stdint.h>

class Bitmap {
public:
    size_t Size;
    uint8_t* Buffer;
    bool operator[](uint64_t index);
    bool Set(uint64_t index, bool value);
    bool Get(uint64_t index);
};

#endif
