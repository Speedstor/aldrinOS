#ifndef BASIC_RENDERER
#define BASIC_RENDERER

#include "Framebuffer.h"
#include "../inc/math.h"
#include "simpleFonts.h"
#include <stdint.h>

class BasicRenderer {
public:

    BasicRenderer(Framebuffer* targetFramebuffer, PSF1_FONT* psf1_Font);

    PSF1_FONT* PSF1_Font;
    uint32_t MouseCursorBuffer[16 * 16];
    uint32_t MouseCursorBufferAfter[16 * 16];
    unsigned int BackgroundColor;
    void Clear();
    void DrawOverlayMouseCursor(uint8_t* mouseCursor, Point opsition, uint32_t color);
    void putPix(uint32_t x, uint32_t y, uint32_t color);
    uint32_t GetPix(uint32_t x, uint32_t y);
    void ClearChar(unsigned int xOff, unsigned int yOff);
    void PutChar(char chr, unsigned int xOff, unsigned int yOff);
    void ClearMouseCursor(uint8_t* mouseCursor, Point position);
    bool MouseDrawn;

    Framebuffer* TargetFramebuffer;
    uint64_t Framebuffer_base;
};

extern BasicRenderer* GlobalRenderer;

#endif
