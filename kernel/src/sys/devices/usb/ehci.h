#ifndef EHCI_H
#define EHCI_H

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
        QH* queueHeadPool;
        qTD* transferDescriptorsPool;
        QH* asyncQueueHead;
        QH* periodicQueueHead;
    };

    class EHCIDriver {
        #define DEFAULT_QH_COUNT 2
        #define DEFAULT_qTD_COUNT 10

        public:
        EHCIDriver(PCI::PCIDeviceHeader* pciBaseAddress);
        ~EHCIDriver();
        PCI::PCIDeviceHeader* pciBaseAddress;
        EhciController hc;
        void enumeratePorts();
        void HandlePortChange_Interrupt();
        void PortSuspend(uint32_t* portStsCtl);
        void PortResume(uint32_t* portStsCtl);
        uint64_t EHCIMemoryPage; //should be 32byte aligned
        uint64_t EHCIMemoryPageEnd; //should be 32byte aligned
        uint64_t freeMemoryPointer;

        private:
        QH* getFreeQueueHead();
        qTD* getqTD();
        void populateTransferDescriptor(
            qTD** pTransferDescriptor, 
            uint8_t dataToggle,
            uint16_t length,
            uint8_t ioc,
            uint8_t countErr,
            uint8_t PID,
            uint8_t status,
            void* data);
        void initAsyncList(QH** pQueueHead);
        void initPeriodicList(QH** pQueueHead, uint32_t** pFrameListBase);
        uint8_t PortChange(uint32_t* portStsCtl);
        void TransferToPort(uint32_t* portStsCtl);



        #if 0 // NOTES
            uint8_t SBRN = *((uint8_t*) ((uint64_t) pciBaseAddress + PCI_SBRN_OFFSET));
            uint8_t FLADJ = *((uint8_t*) ((uint64_t) pciBaseAddress + PCI_FLADJ_OFFSET));
            uint16_t PWAKECAP = *((uint16_t*) ((uint64_t) pciBaseAddress + PCI_PWAKECAP_OFFSET));
        #endif
    };

    
    namespace HELPER {
        void printOpRegs(EhciOperations* opRegs);
    }
}

#endif