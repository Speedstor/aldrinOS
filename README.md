# Aldrin OS
- This OS started with following the tutorial of from Youtuber [Poncho](https://www.youtube.com/channel/UC15iQ_QzTPxB6yGzzifJfKA)
- After finishing the tutorial, I have been trying to add USB compability. While improving on existing code's organization and efficiency.
- Poncho's Tutorial code Github Repo: [https://github.com/Absurdponcho/PonchoOS](https://github.com/Absurdponcho/PonchoOS)
- current working code in on the dev branch

### Notes
#### Kernel Setup Outline (sequential)
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
- notes
    - qemu existing acpi tables
        - FACP - The Fixed ACPI Description Table starts with the “FACP” signature. 
        - APIC - Application Policy Infrastructure Controller - Multiple APIC Description Table
        - HPET - IA-PC High Precision Event Timer Table. See Links to ACPI-Related Documents under the heading “IA-PC High Precision Event Timer Table”.
        - MCFG - PCI Express Memory-mapped Configuration Space base address description table. PCI Firmware Specification, Revision 3.0.
        - WAET - Windows ACPI Emulated Devices Table**. See Links to ACPI-Related Documents under the heading “Windows ACPI Emulated Devices Table”.
        - BGRT - Boot Graphics Resource Table


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
|LPC|Low Pin Count|
|AHCI|Advanced Host Controller Interface (Protocal to communicate with Serial ATA(SATA) devices|
|UPC|Unit Procedure Call|
|USB SS|USB SuperSpeed|
|USB CDC-ACM|USB Communications Device Class – Abstract Control Model|
|BAR|Base Address Register|
|SBRN|Serial Bus Release Bumber Register|
|MMIO|Memory mapped I/O|

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
- cursorColor & layers
- update pci list/buffer devices


### Resources
- [https://github.com/cfenollosa/os-tutorial](https://github.com/cfenollosa/os-tutorial)
- [https://github.com/minoca/os](https://github.com/minoca/os)
- [https://github.com/cia-foundation/TempleOS/tree/archive/Kernel](https://github.com/cia-foundation/TempleOS/tree/archive/Kernel)


### Docs Websites
- [https://uefi.org/specs/ACPI/6.4/05_ACPI_Software_Programming_Model/ACPI_Software_Programming_Model.html](https://uefi.org/specs/ACPI/6.4/05_ACPI_Software_Programming_Model/ACPI_Software_Programming_Model.html)
- [https://pci-ids.ucw.cz/read/PD/](https://pci-ids.ucw.cz/read/PD/)
