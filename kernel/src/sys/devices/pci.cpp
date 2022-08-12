#include "pci.h"
#include "storage/ahci.h"
#include "../memory/heap.h"
#include "usb/usb.h"

namespace PCI {
    void EnumerateFunction(uint64_t deviceAddress, uint64_t function) {
        uint64_t offset = function << 12;
        uint64_t functionAddress = deviceAddress + offset;
        g_PageTableManager.MapMemory((void*)functionAddress, (void*)functionAddress);

            PCIDeviceHeader* pciDeviceHeader = (PCIDeviceHeader*)functionAddress;

            if (pciDeviceHeader->DeviceID == 0) return;
            if (pciDeviceHeader->DeviceID == 0xFFFF) return;

            PRINT::Println("");

            PRINT::Print(GetVendorName(pciDeviceHeader->VendorID));
            PRINT::Print(" | ");
            PRINT::Print(GetDeviceName(pciDeviceHeader->VendorID, pciDeviceHeader->DeviceID));
            PRINT::Print(" | ");
			PRINT::Print(DeviceClasses[pciDeviceHeader->Class]);
            PRINT::Print(" | ");
			PRINT::Print(GetSubclassName(pciDeviceHeader->Class, pciDeviceHeader->Subclass));
            PRINT::Print(" | ");
			PRINT::Print(GetProgIFName(pciDeviceHeader->Class, pciDeviceHeader->Subclass, pciDeviceHeader->ProgIF));
            PRINT::Next();

            PRINT::Print(to_hstring((uint32_t) pciDeviceHeader->VendorID));
            PRINT::Print(" - ");
            PRINT::Print(to_hstring((uint32_t) pciDeviceHeader->DeviceID));
            PRINT::Print(" - ");
            PRINT::Print(to_hstring((uint32_t) pciDeviceHeader->Class));
            PRINT::Print(" - ");
            PRINT::Print(to_hstring((uint32_t) pciDeviceHeader->Subclass));
            PRINT::Print(" - ");
            PRINT::Print(to_hstring((uint32_t) pciDeviceHeader->ProgIF));
            PRINT::Next();

            switch (pciDeviceHeader->Class) {
                case 0x01: // mass storage controller
                    switch (pciDeviceHeader->Subclass) {
                        case 0x06: //serial ATA:
                            switch (pciDeviceHeader->ProgIF) {
                                case 0x01: //AHCI 1.0 device
								    new AHCI::AHCIDriver(pciDeviceHeader);
                                    break;	
                            }
                    }
                case 0x0C: // Serial bus controller
                    switch (pciDeviceHeader->Subclass) {
                        case 0x03: // USB controller
                            switch (pciDeviceHeader->ProgIF) {
                                case 0x00: // UHCI
                                    new USB::UHCIDriver(pciDeviceHeader);
                                    break;
                                case 0x20: // EHCI
                                    new USB::EHCIDriver(pciDeviceHeader);
                                    break;
                            }
                    }
            }
        }

        void EnumerateDevice(uint64_t busAddress, uint64_t device){
            uint64_t offset = device << 15;
            uint64_t deviceAddress = busAddress + offset;
            g_PageTableManager.MapMemory((void*)deviceAddress, (void*)deviceAddress);

            PCIDeviceHeader* pciDeviceHeader = (PCIDeviceHeader*)deviceAddress;

            PRINT::Print("Listing PCI Device Functions:");

            if (pciDeviceHeader->DeviceID == 0) return;
            if (pciDeviceHeader->DeviceID == 0xFFFF) return;

            for (uint64_t function = 0; function < 8; function++) {
                EnumerateFunction(deviceAddress, function);
            }
        }

        void EnumerateBus(uint64_t baseAddress, uint64_t bus) {
            uint64_t offset = bus << 20;
            uint64_t busAddress = baseAddress + offset;
            g_PageTableManager.MapMemory((void*)busAddress, (void*)busAddress);

            PCIDeviceHeader* pciDeviceHeader = (PCIDeviceHeader*)busAddress;

            if (pciDeviceHeader->DeviceID == 0) return;
            if (pciDeviceHeader->DeviceID == 0xFFFF) return;

            for (uint64_t device = 0; device < 32; device++) {
                EnumerateDevice(busAddress, device);
            }
        }

        void EnumeratePCI(ACPI::MCFGHeader* mcfg){
            int entries = ((mcfg->Header.Length) - sizeof(ACPI::MCFGHeader)) / sizeof(ACPI::DeviceConfig);
            for (int t = 0; t < entries; t++){
                ACPI::DeviceConfig* newDeviceConfig = (ACPI::DeviceConfig*)((uint64_t)mcfg + sizeof(ACPI::MCFGHeader) + (sizeof(ACPI::DeviceConfig) * t));
            for (uint64_t bus = newDeviceConfig->StartBus; bus < newDeviceConfig->EndBus; bus++){
                EnumerateBus(newDeviceConfig->BaseAddress, bus);
            }
        }
    }

}
