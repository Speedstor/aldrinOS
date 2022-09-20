#ifndef UHCI_H
#define UHCI_H

#include <stdint.h>
#include <stddef.h>
#include "../pci.h"
#include "EhciStructs.h"

namespace USB {


    class UHCIDriver {
    public:
        UHCIDriver(PCI::PCIDeviceHeader* pciBaseAddress);
        ~UHCIDriver();
        PCI::PCIDeviceHeader* pciBaseAddress;
    };
    
}


#endif