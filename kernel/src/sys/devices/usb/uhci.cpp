/*
    Heavily referenced from: https://github.com/pdoane/osdev
*/
#include "uhci.h"
#include "../../../graphics/Print.h"
#include "../../../inc/cstr.h"
#include "../../memory/heap.h"
#include "../../memory/memory.h"
#include "../IO.h"

namespace USB {

    UHCIDriver::UHCIDriver(PCI::PCIDeviceHeader* pciBaseAddress) {
        this->pciBaseAddress = pciBaseAddress;
        // PRINT::Println("Initializing USB root device");
    }
}