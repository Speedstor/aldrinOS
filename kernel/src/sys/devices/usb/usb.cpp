#include "usb.h"
#include "../../../graphics/Print.h"
#include "../../../inc/cstr.h"

namespace USB {



    UHCIDriver::UHCIDriver(PCI::PCIDeviceHeader* pciBaseAddress) {
        this->pciBaseAddress = pciBaseAddress;
        // PRINT::Println("Initializing USB root device");
    }

    EHCIDriver::EHCIDriver(PCI::PCIDeviceHeader* pciBaseAddress) {
        this->pciBaseAddress = pciBaseAddress;

        UsbBase = ((PCI::PCIHeader0*)pciBaseAddress)->BAR0;
        // UsbOperateBase = *((uint8_t*) UsbBase + EHCI_CAPABILITY_LENGTH_REGISTER);
        PRINT::Next();
        PRINT::Println("Initializing USB EHCI root device !!");
        PRINT::Next();

        SBRN = *((uint8_t*) (uint64_t) pciBaseAddress + PCI_SBRN_OFFSET);
        FLADJ = *((uint8_t*) (uint64_t) pciBaseAddress + PCI_FLADJ_OFFSET);
        PWAKECAP = *((uint16_t*) (uint64_t) pciBaseAddress + PCI_PWAKECAP_OFFSET);
        // Version = *((uint8_t*) UsbBase + EHCI_CAPABILITY_VERSION_REGISTER);
        PRINT::Println(to_hstring(SBRN));
        PRINT::Println(to_hstring(FLADJ));
        PRINT::Println(to_bstring(PWAKECAP));
        PRINT::Next();
    }
}