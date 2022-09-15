

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

    void EHCIDriver::PortSuspend(PortSC* portStsCtl) {
        // wait for queue to finish
        portStsCtl->Suspend = 1;
        // optional: if whole bus is suspended, can change PCI power management
    }

    void EHCIDriver::PortResume(PortSC* portStsCtl) {
        if (portStsCtl->Suspend == 1 && !(hc.opRegs->UsbSts & USBSTS_HCHALTED)) {
            portStsCtl->ForcePortResume = 1;
            PIT::Sleep(20);
            portStsCtl->ForcePortResume = 0;
            portStsCtl->Suspend = 0;
        }
    }

    void EHCIDriver::PortChange(PortSC* portStsCtl) {
        // request to reset and enable the port
        portStsCtl->PortEnableChange = 0;
        portStsCtl->OvercurrentChange = 0;
        portStsCtl->PortReset = 1;
        PIT::Sleep(50);
        portStsCtl->PortReset = 0;
        
        // wait and handle for reset
        PRINT::Next();
        PRINT::Print("Waiting for USB port reset");
        for (uint8_t t = 0; t < 100; t++) {
            PIT::Sleep(10);
            PRINT::Print(".");

            if (portStsCtl->CurrentConnectStatus == 0) { //no device
                PRINT::Print(" No Device");
                break;
            }
            if (portStsCtl->PortEnableChange == 1) {
                PRINT::Println(" Device Enable Change ");
                portStsCtl->PortEnableChange = 0;
            }
            PRINT::Print(" ");
            PRINT::Print(to_string((uint64_t) portStsCtl->PortEnabled));
            PRINT::Print(" ");
            PRINT::Print(to_hstring(*((uint32_t*) portStsCtl)/*  & (1 << 2) */));
            PRINT::Print(" ");
            if (portStsCtl->PortEnabled == 1) { // TODO:: check if port enabled is correct i nthe PortSC struct
                PRINT::Print(" Device Connected");
                break;
            }
        } // resetted 

        // Request GET DEVICE DESCRIPTOR
        #define REQ_SET_ADDR 0x06

        
        
        

        // if USB1.1 (Low- Full- Speed)
        // portStsCtl->PortOwner = 1;
        // else
        // portStsCtl->PortOwner = 0;
    }

    void EHCIDriver::HandlePortChange_Interrupt() {
        uint8_t numPorts = hc.capRegs->structuralParams.NumPorts;
        PortSC* portStsCtls = &hc.opRegs->PortStsCtl;

        for (int8_t i = 0; i < numPorts; i++) {
            if (portStsCtls[i].ConnectStatusChange) {
                portStsCtls[i].ConnectStatusChange = 0;
                PortChange(&portStsCtls[i]);
            }
        }
    }

    void EHCIDriver::enumeratePorts() {
        uint8_t numPorts = hc.capRegs->structuralParams.NumPorts;
        PortSC* portStsCtls = &hc.opRegs->PortStsCtl;

        for (int8_t i = 0; i < numPorts; i++) {
            portStsCtls[i].ConnectStatusChange = 0;
            PortChange(&portStsCtls[i]);
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

    void EHCIDriver::TransferToPort(PortSC* portStsCtl) {
        //the host controller traverses the periodic schedule by constructing an array offset reference from the PERIODICLISTBASE and the FRINDEX registers
        
    }

    EHCIDriver::EHCIDriver(PCI::PCIDeviceHeader* pciBaseAddress) {
        // Initing init
        this->pciBaseAddress = pciBaseAddress;
        uint64_t baseAddress = ((PCI::PCIHeader0*)pciBaseAddress)->BAR0;
        hc.capRegs = (EhciCapbilites*) baseAddress;

        g_PageTableManager.MapMemory(hc.capRegs, hc.capRegs);

        PRINT::Next();
        PRINT::Println("Initializing USB EHCI root device !!");
        PRINT::Next();

        /*
            p.41 https://www.mindshare.com/files/resources/mindshare_ehci_whitepaper.pdf
            p.53 https://www.intel.com/content/dam/www/public/us/en/documents/technical-specifications/ehci-specification-for-usb.pdf
        */
        hc.queueHeadPool = (EhciQueueHead*) malloc(sizeof(EhciQueueHead) * MAX_QH);
        memset(hc.queueHeadPool, 0, sizeof(EhciQueueHead) * MAX_QH);

        PRINT::Println("Initialized queue head pool");

        initAsyncQueueHead(hc.asyncQueueHead);
        initPeriodicQueueHead(hc.periodicQueueHead);
        hc.frameList = initFrameList(hc.opRegs);

        PRINT::Println("Initialized queueHeads");
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

        PRINT::Println("Started USB Host");

        //wait
        PIT::Sleep(5);
        PRINT::Println("Going to enumerate Port");
        enumeratePorts();


        // check capabilities options to set settings for the host controller software


    }
}