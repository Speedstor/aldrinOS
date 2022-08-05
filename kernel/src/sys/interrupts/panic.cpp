#include "panic.h"
#include "../../graphics/BasicRenderer.h"

void Panic(const char* panicMessage) {
    /* GlobalRenderer->ClearColor = 0x0000ff; */
    /* GlobalRenderer->Clear(); */

    /* GlobalRenderer->CursorPosition = {0, 0}; */

    GlobalRenderer->Color = 0x00ffffff;

    GlobalRenderer->Print("Kernel Panic");

    GlobalRenderer->Next();
    GlobalRenderer->Next();

    GlobalRenderer->Print(panicMessage);
}


