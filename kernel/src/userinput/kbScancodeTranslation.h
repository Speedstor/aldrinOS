#ifndef KB_SCANCODE_TRANSLATION_H
#define KB_SCANCODE_TRANSLATION_H

#include <stdint.h>

namespace QWERTYKeyboard {

    #define LeftShift 0x2A
    #define RightShift 0x36
    #define Enter 0x1C
    #define BackSpace 0x0E
    #define Space 0x39

    extern const char ASCIITable[];
    char Translate(uint8_t scancode, bool uppercase);
}


#endif
