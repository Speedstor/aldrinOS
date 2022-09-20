#include "BasicRenderer.h"
#include "Print.h"

BasicRenderer* GlobalRenderer;

BasicRenderer::BasicRenderer(Framebuffer* targetFramebuffer, PSF1_FONT* psf1_Font) {
    TargetFramebuffer = targetFramebuffer;
    PSF1_Font = psf1_Font;
    BackgroundColor = 0xff000000;
    Framebuffer_base = (uint64_t) TargetFramebuffer->BaseAddress;

    PrintVars.Color = 0xffffffff;
    PrintVars.CursorPosition = {0, 0};
    PrintVars.GlyphBuffer = psf1_Font->glyphBuffer;
    PrintVars.CharSize = psf1_Font->psf1_Header->charsize;
    PrintVars.Width = TargetFramebuffer->Width;
    PrintVars.Height = TargetFramebuffer->Height;
}

void BasicRenderer::Clear() {
    uint64_t bytesPerScanline = TargetFramebuffer->PixelsPerScanLine * 4;
    uint64_t fbHeight = TargetFramebuffer->Height;
    uint64_t fbSize = TargetFramebuffer->BufferSize;

    for (int verticalScanline = 0; verticalScanline < fbHeight; verticalScanline++) {
        uint64_t pixPtrBase = Framebuffer_base + (bytesPerScanline * verticalScanline);
        for (uint32_t* pixPtr = (uint32_t*)pixPtrBase; pixPtr < (uint32_t*)(pixPtrBase + bytesPerScanline); pixPtr++) {
            *pixPtr = BackgroundColor;
        }
    }
}

void BasicRenderer::PutChar(char chr, unsigned int xOff, unsigned int yOff) {
    unsigned int* pixPtr = (unsigned int*)TargetFramebuffer->BaseAddress;
    char* fontPtr = (char*) PrintVars.GlyphBuffer + (chr * PrintVars.CharSize);
    for (unsigned long y = yOff; y < yOff + 16; y++) {
        for (unsigned long x = xOff; x < xOff + 8; x++) {
            if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0) {//selecting the bit in the bitmap 
                *(unsigned int*)(pixPtr + x + (y * TargetFramebuffer->PixelsPerScanLine)) = PrintVars.Color;
            }
        }
        fontPtr++;
    }
}

void BasicRenderer::ClearChar(unsigned int xOff, unsigned int yOff) {
    unsigned int* pixPtr = (unsigned int*)TargetFramebuffer->BaseAddress;
    for (unsigned long y = yOff; y < yOff + 16; y++) {
        for (unsigned long x = xOff - 8; x < xOff; x++) {
            *(unsigned int*)(pixPtr + x + (y * TargetFramebuffer->PixelsPerScanLine)) = GlobalRenderer->BackgroundColor;
        }
    }
}

void BasicRenderer::putPix(uint32_t x, uint32_t y, uint32_t color) {
    *(uint32_t*)((uint64_t)Framebuffer_base + (x * 4) + (y * TargetFramebuffer->PixelsPerScanLine * 4)) = color;
}

uint32_t BasicRenderer::GetPix(uint32_t x, uint32_t y) {
    return *(uint32_t*)((uint64_t)TargetFramebuffer->BaseAddress + (x * 4) + (y * TargetFramebuffer->PixelsPerScanLine * 4));
}

void BasicRenderer::ClearMouseCursor(uint8_t* mouseCursor, Point position) {
    if (!MouseDrawn) return;
    int xMax = 16;
    int yMax = 16;
    int differenceX = TargetFramebuffer->Width - position.X;
    int differenceY = TargetFramebuffer->Height - position.Y;

    if (differenceX < 16) xMax = differenceX;
    if (differenceY < 16) yMax = differenceY;

    for (int y = 0; y < yMax; y++) {
        for (int x = 0; x < xMax ; x++) {
            int bit = y * 16 + x;
            int byte = bit / 8;
            if ((mouseCursor[byte] & (0b10000000 >> (x%8)))) {
                if (GetPix(position.X + x, position.Y + y) == MouseCursorBufferAfter[x + y * 16]) {
                    putPix(position.X + x, position.Y + y, MouseCursorBuffer[x + y * 16]);
                }
            }
        }
    }
}

void BasicRenderer::DrawOverlayMouseCursor(uint8_t* mouseCursor, Point position, uint32_t color) {
    MouseDrawn = true;
    int xMax = 16;
    int yMax = 16;
    int differenceX = TargetFramebuffer->Width - position.X;
    int differenceY = TargetFramebuffer->Height - position.Y;

    if (differenceX < 16) xMax = differenceX;
    if (differenceY < 16) yMax = differenceY;

    for (int y = 0; y < yMax; y++) {
        for (int x = 0; x < xMax ; x++) {
            int bit = y * 16 + x;
            int byte = bit / 8;
            if ((mouseCursor[byte] & (0b10000000 >> (x%8)))) {
                MouseCursorBuffer[x+y * 16] = GetPix(position.X + x , position.Y + y);
                putPix(position.X + x, position.Y + y, color);
                MouseCursorBufferAfter[x+y * 16] = GetPix(position.X + x , position.Y + y);
            }
        }
    }
}

