# PonchoOS Tutorial

![](ponchoOS_summary.png)  

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


