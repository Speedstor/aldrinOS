#ifndef PRINT_H
#define PRINT_H

#include "../inc/math.h"
#include "BasicRenderer.h"

namespace PRINT {
    void PutChar(char chr, unsigned int xOff, unsigned int yOff);    
    void Println(const char* str);
    void Print(const char* str);
    void PutChar(char chr);
    void Next();
    void ClearChar();
};

struct PRINT_VARS {
    Point CursorPosition;
    unsigned int Color;
    void* GlyphBuffer;
    unsigned char CharSize;
    long Width;
    long Height;
};

extern PRINT_VARS PrintVars;

#endif