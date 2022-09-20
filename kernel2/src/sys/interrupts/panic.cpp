#include "panic.h"
#include "../../graphics/BasicRenderer.h"

void Panic(const char* panicMessage) {
    /* GlobalRenderer->BackgroundColor = 0x0000ff; */
    /* GlobalRenderer->Clear(); */

    /* PrintVars.CursorPosition = {0, 0}; */

    PrintVars.Color = 0x00ffffff;
    PRINT::Println("Kernel Panic");

    PRINT::Next();

    PRINT::Println(panicMessage);
}


