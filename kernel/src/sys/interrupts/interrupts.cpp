#include "interrupts.h"
#include "panic.h"
#include "../devices/IO.h"
#include "../../userinput/keyboard.h"
#include "../time/pit.h"
#include "../../graphics/Print.h"

__attribute__((interrupt)) void PageFault_handler(struct interrupt_frame* frame) {
    Panic("Page Fault Raised");
    while (true); //halt
}

__attribute__((interrupt)) void DoubleFault_handler(struct interrupt_frame* frame) {
    Panic("Double Fault Detected");
    while (true); //halt
}

__attribute__((interrupt)) void GPFault_handler(struct interrupt_frame* frame) {
    Panic("General Fault Detected");
    while (true); //halt
}

__attribute__((interrupt)) void KeyboardInt_handler(struct interrupt_frame* frame) {
    uint8_t scancode = inb(0x60);
    
    HandleKeyboard(scancode);

    PIC_EndMaster();
}

__attribute__((interrupt)) void MouseInt_handler(struct interrupt_frame* frame) {
    uint8_t mouseData = inb(0x60);

    HandlePS2Mouse(mouseData);

    PIC_EndSlave();
}



__attribute__((interrupt)) void PITInt_handler(struct interrupt_frame* frame) {
    PIT::Tick();
    PIC_EndMaster();
}

void PIC_EndMaster() {
    outb(PIC1_COMMAND, PIC_EOI);
}

void PIC_EndSlave() {
    outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}

void RemapPIC() {
    uint8_t a1, a2;

    a1 = inb(PIC1_DATA);
    io_wait();
    a2 = inb(PIC2_DATA);
    io_wait();

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_TCW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_TCW4);
    io_wait();

    outb(PIC1_DATA, 0x20);
    io_wait();
    outb(PIC2_DATA, 0x28);
    io_wait();

    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    outb(PIC1_DATA, a1);
    io_wait();
    outb(PIC2_DATA, a2);
    io_wait();
}
