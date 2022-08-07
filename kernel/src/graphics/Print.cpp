#include "Print.h"

PRINT_VARS PrintVars;

namespace PRINT{
    void Next() {
        PrintVars.CursorPosition.X = 0;
        PrintVars.CursorPosition.Y += 16;
    }

    void PutChar(char chr) {
        GlobalRenderer->PutChar(chr, PrintVars.CursorPosition.X, PrintVars.CursorPosition.Y);
        PrintVars.CursorPosition.X += 8;
        if (PrintVars.CursorPosition.X + 8 > PrintVars.Width) {
            PrintVars.CursorPosition.X = 0;
            PrintVars.CursorPosition.Y += 16;
        }
    }

    void Print(const char* str) {
        char* chr = (char*)str;
        while (*chr != 0) {
            PutChar(*chr);
            chr++;
        }
    }

    void ClearChar() {
        if (PrintVars.CursorPosition.X == 0) {
            PrintVars.CursorPosition.X = PrintVars.Width;
            PrintVars.CursorPosition.Y -= 16;
            if (PrintVars.CursorPosition.Y < 0) PrintVars.CursorPosition.Y = 0;
        }

        unsigned int xOff = PrintVars.CursorPosition.X;
        unsigned int yOff = PrintVars.CursorPosition.Y;

        GlobalRenderer->ClearChar(xOff, yOff);

        PrintVars.CursorPosition.X -= 8;

        if (PrintVars.CursorPosition.X < 0) {
            PrintVars.CursorPosition.X = PrintVars.Width;
            PrintVars.CursorPosition.Y -= 16;
            if (PrintVars.CursorPosition.Y < 0) PrintVars.CursorPosition.Y = 0;
        }
    }

    void Println(const char* str) {
        Print(str);
        Next();
    }

}
