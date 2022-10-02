

#include "ehci.h"
#include "EhciPacketDef.h"
#include "../../../graphics/Print.h"
#include "../../../inc/cstr.h"
#include "../../memory/heap.h"
#include "../../memory/memory.h"
#include "../IO.h"
#include "../../time/pit.h"
#include "../../memory/paging/PageFrameAllocator.h"

namespace USB {
    void EHCIDriver::populateTransferDescriptor(
        qTD** pTransferDescriptor, 
        uint8_t dataToggle,
        uint16_t length,
        uint8_t ioc,
        uint8_t countErr,
        uint8_t PID,
        uint8_t status, //TODO:: expand?
        void* pData
    ) {
        qTD* transferDescriptor = *pTransferDescriptor;
        
        transferDescriptor->altLink = QTD_NEXT_TERMINATE;

        //TODO:: Optimize below
        uint32_t token = 0;
        if (dataToggle) token |= QTD_TOKEN_DATA_TOGGLE;
        if (ioc) token |= QTD_TOKEN_IOC;
        token |= countErr << QTD_TOKEN_ERROR_COUNTER_SHIFT; //TODO truncate to one line
        token |= length << QTD_TOKEN_LENGTH_SHIFT;
        token |= PID << QTD_TOKEN_PID_SHIFT;
        token |= status;
        transferDescriptor->token = token;

        transferDescriptor->pBuffer[0] = (uint32_t)(uint64_t) pData;
        transferDescriptor->pBuffer[1] = 0;
        transferDescriptor->pBuffer[2] = 0;
        transferDescriptor->pBuffer[3] = 0;
        transferDescriptor->pBuffer[4] = 0;
    }

    // For debug, let it return -1 when the address is not 32bit aligned
    int8_t EHCIDriver::linkDescriptors(qTD* fromTD, qTD* toTD) {
        uint64_t toAddr = (uint64_t) toTD;
        if (toAddr & (UPPER_64_MASK | LEAST_5_BIT_MASK)) {
            PRINT::Next();
            PRINT::Print(to_hstring((uint64_t) (UPPER_64_MASK | LEAST_5_BIT_MASK)));
            PRINT::Print(" toTD addr is: ");
            PRINT::Print(to_hstring(toAddr));
            PRINT::Print(" | ");
            PRINT::Println("linking qTD failed");
            return -1;
        }
        fromTD->link = (uint32_t) toAddr;
        return 0;
    }

    void EHCIDriver::endLinkDescriptor(qTD* endTD) {
        endTD->link = QTD_NEXT_TERMINATE;
    }

    //TODO:: figuring out if deallocating queue heads are possible, and to implement it if so
    QH* EHCIDriver::getFreeQueueHead() {
        QH* avaliableQueueHead = (QH*) freeMemoryPointer;
        freeMemoryPointer += sizeof(QH); //sizeof(QH) should be at least multiple of 32bytes, and should be 64
        if (freeMemoryPointer < EHCIMemoryPageEnd) {
            return avaliableQueueHead;
        }
        return 0;
    }

    int EHCIDriver::waitResponseQH(QH* qh) {
        int err = 1; // TODO:: define err consts

        if (!(hc.opRegs->UsbSts & USBSTS_ASYNCHRONOUS_SCHEDULE_STATUS)) {
            err = 10;
        }
        for (int8_t t = 0; t < 100; t++) { 
            if (qh->token & QTD_TOKEN_STATUS_HALTED) {
                PRINT::Println("ERROR: waiting for async halted");
                err = 2;
                break;
            }
            if ((qh->nextLink & QTD_NEXT_TERMINATE) && !(qh->token & QTD_TOKEN_STATUS_ACTIVE)) {
                if (qh->token & GET_DESCRIPTOR_DATA_BUFFER) {
                    PRINT::Println(" Data Buffer Error\n");
                    err = 3;
                }
                if (qh->token & QTD_TOKEN_STATUS_BABBLE_DETECTED) {
                    PRINT::Println(" Babble Detected\n");
                    err = 4;
                }
                if (qh->token & QTD_TOKEN_STATUS_TRANSACTION_ERROR) {
                    PRINT::Println(" Transaction Error\n");
                    err = 5;
                }
                if (qh->token & QTD_TOKEN_STATUS_MISSED_MFRAME) {
                    PRINT::Println(" Missed Micro-Frame\n");
                    err = 6;
                }

                err = 0;
                break;
            }

            PIT::Sleep(3);
        }

        //TODO:: free qTDs     :)

        if (err != 0) {
            PRINT::Next();
            PRINT::Print("token: ");
            PRINT::Print(to_hstring(qh->token));
            PRINT::Print(" | nextLink: ");
            PRINT::Print(to_hstring(qh->nextLink));
            PRINT::Print(" | hc.opRegs->UsbSts: ");
            PRINT::Print(to_hstring(hc.opRegs->UsbSts));
            PRINT::Next();
            PRINT::Println("ERROR:: waiting for async response timeout");
        }
        return err;
    }
    
    // TODO:: 
    void EHCIDriver::insertAsyncQH(QH* headQH, QH* newQH) {
        
    }
    

    qTD* EHCIDriver::getqTD() {
        qTD* avaliableqTD = (qTD*) freeMemoryPointer;
        freeMemoryPointer += sizeof(qTD);
        if (freeMemoryPointer < EHCIMemoryPageEnd) {
            return avaliableqTD;
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

        PRINT::Print(" Device Connected | ");

        PRINT::Print("portSC now is: ");
        PRINT::Print(to_hstring(*portStsCtl));
        PRINT::Print("  |  ");

        // GET_DESCRIPTOR request (SETUP packet)
        qTD* setupDescriptor = getqTD();
        PRINT::Next();
        PRINT::Println(to_hstring(GET_DESCRIPTOR_DATA_BUFFER));
        //TODO:: check if &GET_DESCRIPTOR_DATA_BUFFER is aross a 4kbyte alignment
        populateTransferDescriptor(
            &setupDescriptor,
            (uint8_t) 0,
            (uint16_t) SETUP_PACKET_LENGTH,
            (uint8_t) 0,
            (uint8_t) QTD_ERR_COUNT,
            (uint8_t) PID_SETUP,
            (uint8_t) QTD_TOKEN_STATUS_ACTIVE,
            (void*) &GET_DESCRIPTOR_DATA_BUFFER
        );
        PRINT::Print("setupDescriptor Token: ");
        PRINT::Print(to_hstring(setupDescriptor->token));
        PRINT::Print("  |  ");

        // IN packet
        qTD* inDescriptor = getqTD();
        uint32_t* recieveData = (uint32_t*) malloc(0x40);
        populateTransferDescriptor(
            &inDescriptor,
            1,
            0x40,
            0,
            QTD_ERR_COUNT,
            PID_IN,
            QTD_TOKEN_STATUS_ACTIVE,
            recieveData
        );
        linkDescriptors(setupDescriptor, inDescriptor);
        PRINT::Print("inDescriptor Token: ");
        PRINT::Print(to_hstring(inDescriptor->token));
        PRINT::Print("  |  ");

        // ACK packet
        qTD* ackDescriptor = getqTD();
        populateTransferDescriptor(
            &ackDescriptor,
            1,
            EMPTY_LENGTH,
            1,
            QTD_ERR_COUNT,
            PID_OUT,
            QTD_TOKEN_STATUS_ACTIVE,
            0
        );
        linkDescriptors(inDescriptor, ackDescriptor);
        endLinkDescriptor(ackDescriptor);
        PRINT::Print("ackDescriptor Token: ");
        PRINT::Print(to_hstring(ackDescriptor->token));
        PRINT::Print("  |  ");

        PRINT::Next();
        PRINT::Print("setup packet addr: ");
        PRINT::Print(to_hstring((uint64_t) setupDescriptor));

        PRINT::Next();
        PRINT::Print("receive IN Data: ");
        PRINT::Print(to_hstring(*recieveData));
        PRINT::Print(" ");
        PRINT::Print(to_hstring(*(recieveData + 1)));

        QH* endpointZeroQH;
        initAsyncList(&endpointZeroQH);

        PRINT::Next();
        PRINT::Print("link addr before: ");
        PRINT::Print(to_hstring((uint64_t) endpointZeroQH->nextLink));

        // link it to async packet
        linkDescriptors((qTD*) &endpointZeroQH->nextLink, setupDescriptor);

        PRINT::Next();
        PRINT::Print("link addr after: ");
        PRINT::Print(to_hstring((uint64_t) endpointZeroQH->nextLink));

        // add it the async list
        endpointZeroQH->qhlp = ((uint32_t)(uint64_t) hc.asyncQueueHead) | QH_TYP_QH;
        hc.asyncQueueHead->qhlp = ((uint32_t)(uint64_t) endpointZeroQH) | QH_TYP_QH;

        // store device descriptor
        int err = waitResponseQH(endpointZeroQH);

        PRINT::Next();
        PRINT::Print("receive IN Data: (");
        PRINT::Print(to_string((uint64_t) err));
        PRINT::Print(") ");
        PRINT::Print(to_hstring(*recieveData));
        PRINT::Print(" ");
        PRINT::Print(to_hstring(*(recieveData + 1)));


        // remove endpointZeroQH


        return 1;
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
                PRINT::Print(to_hstring((uint64_t) &portStsCtls[i]));
                PRINT::Print(" | ");
                PRINT::Print("PortSC hex: ");
                PRINT::Print(to_hstring(portStsCtls[i]));
                
                PortChange((uint32_t*) (portStsCtls + i));
            }
        }
    }


    void EHCIDriver::initAsyncList(QH** pQueueHead) {
        #define HEAD_RELCAMATION_LIST_FLAG 0x00008000
        QH* queueHead = getFreeQueueHead();
        queueHead->qhlp = (uint64_t)(uint32_t*)queueHead | (QH_TYP_QH << QH_TYP_SHIFT);
        queueHead->ch = ((DEFAULT_MAX_PACKET_LENGTH << QH_MAX_PACKET_LENGHT_SHIFT) | QH_HEAD_RECLAMATION_FLAG | QH_DATA_TOGGLE_CONTROL | QH_EPS_HIGH_SPEED);
        queueHead->caps = 0x40000000; //TODO:: make more rigorously defined
        queueHead->curLink = 0;
        queueHead->nextLink = QTD_NEXT_TERMINATE;
        queueHead->altLink = 0;
        queueHead->token = 0;
        for (uint32_t i = 0; i < 5; ++i) {
            queueHead->buffer[i] = 0;
        }
        
        *pQueueHead = queueHead;
    }

    void EHCIDriver::initPeriodicList(QH** pQueueHead, uint32_t** pFrameListBase) {
        // periodic queue head
        QH* queueHead = getFreeQueueHead();
        queueHead->qhlp = QTD_NEXT_TERMINATE;
        queueHead->ch = 0;
        queueHead->caps = 0;
        queueHead->curLink = 0;
        queueHead->nextLink = QTD_NEXT_TERMINATE;
        queueHead->altLink = 0;
        queueHead->token = 0;
        for (uint8_t i = 0; i < 5; ++i) {
            queueHead->buffer[i] = 0;
        }

        // TODO:: right now, it only allows for __1ms__ periodic transfers
        // frame list
        uint32_t* frameList = (uint32_t*) GlobalAllocator.RequestPage();
        g_PageTableManager.MapMemory(frameList, frameList);
        for (uint16_t i = 0; i < 1024; ++i)
        {
            frameList[i] = (QH_TYP_QH << QH_TYP_SHIFT) | (uint64_t)(uint32_t*)queueHead;
        }
        hc.opRegs->FrameIndex = 0;
        hc.opRegs->PeriodicList = (uint64_t) hc.frameList;

        *pQueueHead = queueHead;
        *pFrameListBase = frameList;
    }

    void EHCIDriver::TransferToPort(uint32_t* portStsCtl) {
        //the host controller traverses the periodic schedule by constructing an array offset reference from the PERIODICLISTBASE and the FRINDEX registers
        
    }

    /*
        p.41 https://www.mindshare.com/files/resources/mindshare_ehci_whitepaper.pdf
        p.53 https://www.intel.com/content/dam/www/public/us/en/documents/technical-specifications/ehci-specification-for-usb.pdf
    */
    EHCIDriver::EHCIDriver(PCI::PCIDeviceHeader* pciBaseAddress) {
        // TODO:: explain or make sure that the address can be represented with only lower 32 bits
        EHCIMemoryPage = (uint64_t) GlobalAllocator.RequestPage();
        EHCIMemoryPageEnd = EHCIMemoryPage + 4096;
        freeMemoryPointer = EHCIMemoryPage;

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

        PRINT::Println(to_string(sizeof(QH)));

        initAsyncList(&hc.asyncQueueHead);
        initPeriodicList(&hc.periodicQueueHead, &hc.frameList);

        // control data segment
        hc.opRegs->CtlDataSegement = 0; //TODO:: suppose to map to 4Gigabyte segment
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