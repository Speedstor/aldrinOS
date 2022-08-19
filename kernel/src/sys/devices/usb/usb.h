#ifndef USB_H
#define USB_H

#include <stdint.h>
#include <stddef.h>
#include "../pci.h"
#include "EhciStructs.h"

namespace USB {
    enum PortType {
        TypeA = 0x00,
        MiniAB = 0x01,
        ExpressCard = 0x02,
        USB3_StdA = 0x03,
        USB3_StdB = 0x04,
        USB3_MicroB = 0x05,
        USB3_MicroAB = 0x06,
        USB3_PowerB = 0x07,
        USB2_TypeC = 0x08,
        USB2_SS_TypeC = 0x09, 
        USB2_SS_Switch_TypeC = 0x0A,

        Proprietary = 0xFF,
    };

    struct Package {
        uint8_t Connectable;
        uint8_t Type;
        uint32_t Reserved0 = 0;
        uint32_t Reserved1 = 0;
    };


    #define USB_BASE_OFFSET 0x10
    #define PCI_SBRN_OFFSET 0x60
    #define PCI_FLADJ_OFFSET 0x61
    #define PCI_PWAKECAP_OFFSET 0x62




    class UHCIDriver {
    public:
        UHCIDriver(PCI::PCIDeviceHeader* pciBaseAddress);
        ~UHCIDriver();
        PCI::PCIDeviceHeader* pciBaseAddress;
    };


    struct EhciInfo {
        uint8_t SBRN;
        uint8_t FLADJ;
        uint16_t PWAKECAP;
        uint32_t UsbBase;
        uint32_t UsbOperateBase;
        uint16_t Version;
    };

    struct EhciController
    {
        EhciInfo info;
        EhciCapbilites* capRegs;
        EhciOperations* opRegs;
        uint32_t *frameList;
        EhciQueueHead* queueHeadPool;
        EhciTransferDescriptor* transferDescriptorsPool;
        EhciQueueHead* asyncQueueHead;
        EhciQueueHead* periodicQueueHead;
    };

    class EHCIDriver {
        public:
        EHCIDriver(PCI::PCIDeviceHeader* pciBaseAddress);
        ~EHCIDriver();
        PCI::PCIDeviceHeader* pciBaseAddress;
        EhciController hc;
        void enumeratePorts();

        private:
        EhciQueueHead* getFreeQueueHead();


        #if 0 // NOTES
            uint8_t SBRN = *((uint8_t*) ((uint64_t) pciBaseAddress + PCI_SBRN_OFFSET));
            uint8_t FLADJ = *((uint8_t*) ((uint64_t) pciBaseAddress + PCI_FLADJ_OFFSET));
            uint16_t PWAKECAP = *((uint16_t*) ((uint64_t) pciBaseAddress + PCI_PWAKECAP_OFFSET));
        #endif
    };
}


#endif