# PonchoOS Tutorial

### Notes
#### Poncho OS (sequential)
- __bootloader__ -> prepare struct bootInfo
- __kernel__
    - prepareKernel
        - BasicRenderer
        - GDT //Global Descriptor Table (sections of memory)
        - Prepare memory
            - PageFrameAllocator
            - Reserve EFI Memory
            - Lock Kernel Pages
            - Lock Framebuffer
            - replace EFI Virtual Memory (Page Map?)
        - Prepare interrupts
        - initialize heap (w/ page map)
        - Handle ACPI (devices)
            - get xsdt from rsdp
            - use xsdt to get MCFG
            - use MCFG to get all the devices

#### Brainstorm
- Implement USB devices (keybaord)
- PCI -> Serial Bus
- get access to Serial Bus ids first
- figure out the outb and inb to interface with it


#### bootInfo
```{cpp}
struct bootInfo {
    framebuffer,
    psf1-font,
    EFI_MEMORY_MAP mMap,
    mMapSize,
    mMapDescSize,
    RSDP, //Root System Descriptor Pointer
}
```

#### Acronyms
|acronym|stands for|
|---|---|
|GDT|Global Descriptor Table|
|EFI|Extensible Firmware Interface|
|ACPI|Advanced Configuration and Power Interface|
|RSDP|Root System Descriptor Pointer|
|PCI|Peripheral Component Interconnect|
|IDT|Interrupt Descriptor Table|
|SDT|Service Descriptor Table|
|MCFG|Memory Mapped Configuration Space Access (info about PCI configuration space base)|

### Structure
|folder|sub-folder|what's inside|
|---    |---    |---|
|sys    |boot   |- kernelUtil<br/>- gdt<br/>- efiMemory
|       |memory |- memory<br/>- heap<br/>- paging<br/>- PageFrameAllocator<br/>- PageMapIndexer<br/>- PageTableManager
|       |storage|- ahci<br/>- 
|       |time   |- pit
|       |interrupts|- IDT<br/>- interrupts<br/>- panic<br/>
|       |devices|- apci<br/>- IO<br/>- pci<br/>- pciDescriptors
|userinput|     |- kbScancodeTranslation<br/>- keyboard<br/>- mouse<br/>
|inc    |       |- Bitmap<br/>- cstr<br/>- math<br/>
|graphics|      |- BasicRenderer<br/>- Framebuffer<br/>- simpleFonts
|       |       |- kernel.cpp<br/>

### TODO
- LoadGDT asm
- PCI Serial Bus
- improve printing to screen
- improve ClearChar
- cursorColor & layers