#include "keyboard.h"
#include "../graphics/Print.h"
#include "kbScancodeTranslation.h"


bool isLeftShiftPressed;
bool isRightShiftPressed;

void HandleKeyboard(uint8_t scancode) {
    switch(scancode) {
    case LeftShift:
        isLeftShiftPressed = true;
        return;
    case LeftShift + 0x80:
        isLeftShiftPressed = false;
        return;
    case RightShift:
        isRightShiftPressed = true;
        return;
    case RightShift + 0x80:
        isRightShiftPressed = false;
        return;
    case Enter:
        PRINT::Next();
        return;
    case Space:
        PRINT::PutChar(' ');
        return;
    case BackSpace:
        PRINT::ClearChar();
        return;
    }
    char ascii = QWERTYKeyboard::Translate(scancode, isLeftShiftPressed | isRightShiftPressed);

    if (ascii != 0) {
        PRINT::PutChar(ascii);
    }
}

