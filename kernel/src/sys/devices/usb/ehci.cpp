

#include "ehci.h"
#include "../../../graphics/Print.h"
#include "../../../inc/cstr.h"
#include "../../memory/heap.h"
#include "../../memory/memory.h"
#include "../IO.h"
#include "../../time/pit.h"

namespace USB {
    
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

    void EHCIDriver::PortSuspend(uint32_t* portStsCtl) {
        // wait for queue to finish
        *portStsCtl |= EHCI_PORT_SUSPEND;
        // optional: if whole bus is suspended, can change PCI power management
    }

    void EHCIDriver::PortResume(uint32_t* portStsCtl) {
        if (*portStsCtl &= EHCI_PORT_SUSPEND && !(hc.opRegs->UsbSts & USBSTS_HCHALTED)) {
            *portStsCtl |= EHCI_PORT_FORCE_PORT_RESUME;
            PIT::Sleep(20);
            *portStsCtl &= ~(EHCI_PORT_FORCE_PORT_RESUME | EHCI_PORT_SUSPEND);
        }
    }

    uint8_t EHCIDriver::PortChange(uint32_t* portStsCtl) {
        uint32_t bufPortSC = *portStsCtl;

        if (!(bufPortSC & EHCI_PORT_CONNECTION)) { //no device
            PRINT::Println("No Device Connected");
            return 0;
        }
        if ((bufPortSC & (EHCI_PORT_ENABLE | EHCI_PORT_CONNECTION)) == 0 &&
            (bufPortSC & EHCI_PORT_LS_MASK) == 0b01) {
            PRINT::Println("Port is low speed"); // USB1.1 (Low- Speed)
            *portStsCtl |= EHCI_PORT_OWNER;
            return 2;
        }

        //reset
        bufPortSC &= ~(EHCI_PORT_ENABLE_CHANGE | EHCI_PORT_OVER_CURRENT_CHANGE |
                        EHCI_PORT_ENABLE | EHCI_PORT_CONNECTION_CHANGE);
        bufPortSC |= EHCI_PORT_RESET;
        *portStsCtl = bufPortSC;
        PIT::Sleep(50);
        *portStsCtl &= ~EHCI_PORT_RESET;
        PIT::Sleep(10);

        if ((*(uint32_t*) portStsCtl) & EHCI_PORT_ENABLE_CHANGE) {
            PRINT::Println(" Device Enable Change ");
            bufPortSC = *portStsCtl;
            bufPortSC &= ~EHCI_PORT_ENABLE_CHANGE;
            *portStsCtl = bufPortSC;
        }
        if (!(*(uint32_t*) portStsCtl) & (1 << 2))
            return 0;

        PRINT::Println(" Device Connected");

        PRINT::Print("portSC now is: ");
        PRINT::Print(to_hstring(*portStsCtl));
        PRINT::Print("  |  ");

        // Request GET DEVICE DESCRIPTOR
        #define REQ_SET_ADDR 0x06

        // set up async queue head
        

        // store device descriptor


    }

    void EHCIDriver::HandlePortChange_Interrupt() {
        uint8_t numPorts = hc.capRegs->structuralParams.NumPorts;
        uint32_t* portStsCtls = &hc.opRegs->PortStsCtl;

        for (int8_t i = 0; i < numPorts; i++) {
            if (*(portStsCtls + i) & EHCI_PORT_CONNECTION_CHANGE) {
                PortChange((uint32_t*) (portStsCtls + i));
            }
        }
    }

    void EHCIDriver::enumeratePorts() {
        uint8_t numPorts = hc.capRegs->structuralParams.NumPorts;
        uint32_t* portStsCtls = (uint32_t*) ((uint64_t)hc.opRegs + EHCI_PORTSC_OFFSET);
        PRINT::Println(to_hstring((uint64_t) portStsCtls));

        for (int8_t i = 0; i < numPorts; i++) {
            if (*(portStsCtls + i) & EHCI_PORT_CONNECTION_CHANGE) {
                PRINT::Print("PortSC base addr: ");
                PRINT::Print(to_hstring(*(uint32_t*)&portStsCtls[i]));
                PRINT::Print(" | ");
                PRINT::Print("PortSC hex: ");
                PRINT::Print(to_hstring(portStsCtls[i]));
                
                PortChange((uint32_t*) (portStsCtls + i));
            }
        }
    }

    #define SET_TERMINATE (1 << 0)
    #define SET_QH_FLAG (1 << 1)

    uint32_t* EHCIDriver::initFrameList(EhciOperations* opRegs) {
        // frame list
        uint32_t* frameList = (uint32_t*) malloc(1024 * sizeof(uint32_t));
        for (uint16_t i = 0; i < 1024; ++i)
        {
            frameList[i] = SET_QH_FLAG | (uint64_t)(uint32_t*)hc.periodicQueueHead;
        }
        hc.opRegs->FrameIndex = 0;
        hc.opRegs->PeriodicList = (uint64_t) hc.frameList;

        return frameList;
    }

    void EHCIDriver::initAsyncQueueHead(EhciQueueHead* queueHead) {
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
    }

    void EHCIDriver::initPeriodicQueueHead(EhciQueueHead* queueHead) {
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
    }

    void EHCIDriver::TransferToPort(uint32_t* portStsCtl) {
        //the host controller traverses the periodic schedule by constructing an array offset reference from the PERIODICLISTBASE and the FRINDEX registers
        
    }

    /*
        p.41 https://www.mindshare.com/files/resources/mindshare_ehci_whitepaper.pdf
        p.53 https://www.intel.com/content/dam/www/public/us/en/documents/technical-specifications/ehci-specification-for-usb.pdf
    */
    EHCIDriver::EHCIDriver(PCI::PCIDeviceHeader* pciBaseAddress) {
        // Initing init
        this->pciBaseAddress = pciBaseAddress;
        uint64_t baseAddress = ((PCI::PCIHeader0*)pciBaseAddress)->BAR0;
        hc.capRegs = (EhciCapbilites*) baseAddress;

        g_PageTableManager.MapMemory(hc.capRegs, hc.capRegs);
        hc.opRegs = (EhciOperations*) (baseAddress + hc.capRegs->length);
        g_PageTableManager.MapMemory(hc.opRegs, hc.opRegs);

        PRINT::Next();
        PRINT::Print("Initializing USB EHCI root device !!: opRegs: ");
        PRINT::Println(to_hstring((uint64_t) hc.opRegs));
        PRINT::Next();

        hc.queueHeadPool = (EhciQueueHead*) malloc(sizeof(EhciQueueHead) * MAX_QH);
        memset(hc.queueHeadPool, 0, sizeof(EhciQueueHead) * MAX_QH);

        initAsyncQueueHead(hc.asyncQueueHead);
        initPeriodicQueueHead(hc.periodicQueueHead);
        hc.frameList = initFrameList(hc.opRegs);


        // control data segment
        hc.opRegs->CtlDataSegement = 0; //TODO:: suppose to map to 4Gigabyte segment

        // transfer descriptors
        hc.transferDescriptorsPool = (EhciTransferDescriptor*) malloc(sizeof(EhciTransferDescriptor) * MAX_TD);
        memset(hc.transferDescriptorsPool, 0, sizeof(EhciTransferDescriptor) * MAX_TD);

        hc.opRegs->UsbIntr = 1; // TODO:: Handle interrupts

        #define CMD_ITC_SHIFT 16 //Interrupt Threshold Control
        #define CMD_PSE (1 << 4) //Periodic Schedule Enable
        #define CMD_ASE (1 << 5) //Asynchronous Schedule Enable
        #define CMD_RS (1 << 0) //Run/Stop - Run Host Controller proceeds with execution of the schedule
        hc.opRegs->UsbCmd = (8 << CMD_ITC_SHIFT) | CMD_PSE | CMD_ASE | CMD_RS;
        hc.opRegs->ConfigFlag = 1; //start controller

        HELPER::printOpRegs(hc.opRegs);

        PRINT::Println("Started USB Host");

        //wait
        PIT::Sleep(5);
        PRINT::Println("Going to enumerate Port");
        enumeratePorts();


        // check capabilities options to set settings for the host controller software


    }

    namespace HELPER {
        void printOpRegs(EhciOperations* opRegs) {
            PRINT::Println("OpRegs: ");
            uint32_t* intPrint = (uint32_t*) opRegs;
            for (int i = 0; i < 18; i++) {
                PRINT::Print(to_hstring(intPrint[i]));
                PRINT::Print(" ");
                if (i % 10 == 9) {
                    PRINT::Next();
                }
            }
            PRINT::Next();
        }
    }
}