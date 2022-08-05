#ifndef BASIC_RENDERER
#define BASIC_RENDERER

#include "Framebuffer.h"
#include "math.h"
#include "simpleFonts.h"
#include <stdint.h>

class BasicRenderer {
public:

    BasicRenderer(Framebuffer* targetFramebuffer, PSF1_FONT* psf1_Font);
        
    Point CursorPosition;
    Framebuffer* TargetFramebuffer;
    PSF1_FONT* PSF1_Font;
    uint32_t MouseCursorBuffer[16 * 16];
    uint32_t MouseCursorBufferAfter[16 * 16];
    unsigned int Color;
    unsigned int ClearColor;
    void PutChar(char chr, unsigned int xOff, unsigned int yOff);
    void Print(const char* str);
    void Clear();
    void Next();
    void PutChar(char chr);
    void ClearChar();
    void DrawOverlayMouseCursor(uint8_t* mouseCursor, Point opsition, uint32_t color);
    void putPix(uint32_t x, uint32_t y, uint32_t color);
    uint32_t GetPix(uint32_t x, uint32_t y);
    void ClearMouseCursor(uint8_t* mouseCursor, Point position);
    bool MouseDrawn;
};

extern BasicRenderer* GlobalRenderer;

#endif
