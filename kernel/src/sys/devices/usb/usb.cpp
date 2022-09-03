/*
    Heavily referenced from: https://github.com/pdoane/osdev
*/
#include "usb.h"
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

    #define MAX_QH  16   //TODO:: make 1024
    #define MAX_TD  32
    EhciQueueHead* EHCIDriver::getFreeQueueHead() {
        EhciQueueHead* pEnd = hc.queueHeadPool + MAX_QH;
        for (EhciQueueHead* qh = hc.queueHeadPool; qh != pEnd; qh++) {
            if (!qh->active) {
                qh->active = 1;
                return qh;
            }
        }
        return 0;
    }

    void EHCIDriver::enumeratePorts() {

    }

    EHCIDriver::EHCIDriver(PCI::PCIDeviceHeader* pciBaseAddress) {
        this->pciBaseAddress = pciBaseAddress;

        uint64_t baseAddress = ((PCI::PCIHeader0*)pciBaseAddress)->BAR0;
        hc.capRegs = (EhciCapbilites*) baseAddress;

        g_PageTableManager.MapMemory(hc.capRegs, hc.capRegs);

        PRINT::Next();
        PRINT::Println("Initializing USB EHCI root device !!");
        PRINT::Next();

        // Follow documentation from
        /*
            p.41 https://www.mindshare.com/files/resources/mindshare_ehci_whitepaper.pdf
            p.53 https://www.intel.com/content/dam/www/public/us/en/documents/technical-specifications/ehci-specification-for-usb.pdf
        */
        hc.queueHeadPool = (EhciQueueHead*) malloc(sizeof(EhciQueueHead) * MAX_QH);
        memset(hc.queueHeadPool, 0, sizeof(EhciQueueHead) * MAX_QH);

        #define SET_TERMINATE (1 << 0)
        #define SET_QH_FLAG (1 << 1)
        EhciQueueHead* queueHead;

        //async queue head
        #define HEAD_RELCAMATION_LIST_FLAG 0x00008000
        queueHead = getFreeQueueHead();
        queueHead->qhlp = (uint64_t)(uint32_t*)queueHead | SET_QH_FLAG;
        queueHead->ch = HEAD_RELCAMATION_LIST_FLAG;
        queueHead->caps = 0;
        queueHead->curLink = 0;
        queueHead->nextLink = SET_TERMINATE;
        queueHead->altLink = 0;
        queueHead->token = 0;
        for (uint32_t i = 0; i < 5; ++i) {
            queueHead->buffer[i] = 0;
            queueHead->extBuffer[i] = 0;
        }
        queueHead->transfer = 0;
        queueHead->qhLink.prev = &queueHead->qhLink;
        queueHead->qhLink.next = &queueHead->qhLink;

        hc.asyncQueueHead = queueHead;

        // periodic queue head
        queueHead = getFreeQueueHead();
        queueHead->qhlp = SET_TERMINATE;
        queueHead->ch = 0;
        queueHead->caps = 0;
        queueHead->curLink = 0;
        queueHead->nextLink = SET_TERMINATE;
        queueHead->altLink = 0;
        queueHead->token = 0;
        for (uint8_t i = 0; i < 5; ++i) {
            queueHead->buffer[i] = 0;
            queueHead->extBuffer[i] = 0;
        }
        queueHead->transfer = 0;
        queueHead->qhLink.prev = &queueHead->qhLink;
        queueHead->qhLink.next = &queueHead->qhLink;

        hc.periodicQueueHead = queueHead;

        // frame list
        hc.frameList = (uint32_t*) malloc(1024 * sizeof(uint32_t));
        for (uint16_t i = 0; i < 1024; ++i)
        {
            hc.frameList[i] = SET_QH_FLAG | (uint64_t)(uint32_t*)hc.periodicQueueHead;
        }
        hc.opRegs->FrameIndex = 0;
        hc.opRegs->PeriodicList = (uint64_t) hc.frameList;

        // control data segment
        hc.opRegs->CtlDataSegement = 0; //TODO:: suppose to map to 4Gigabyte segment

        // transfer descriptors
        hc.transferDescriptorsPool = (EhciTransferDescriptor*) malloc(sizeof(EhciTransferDescriptor) * MAX_TD);
        memset(hc.transferDescriptorsPool, 0, sizeof(EhciTransferDescriptor) * MAX_TD);

        // disable interrupts TODO:: ?? would want interrupts i think
        hc.opRegs->UsbIntr = 0;

        #define CMD_ITC_SHIFT 16 //Interrupt Threshold Control
        #define CMD_PSE (1 << 4) //Periodic Schedule Enable
        #define CMD_ASE (1 << 5) //Asynchronous Schedule Enable
        #define CMD_RS (1 << 0) //Run/Stop - Run Host Controller proceeds with execution of the schedule
        hc.opRegs->UsbCmd = (8 << CMD_ITC_SHIFT) | CMD_PSE | CMD_ASE | CMD_RS;
        hc.opRegs->ConfigFlags = 1; //start controller

        //wait
        io_wait();
        enumeratePorts();


        // check capabilities options to set settings for the host controller software


    }
}