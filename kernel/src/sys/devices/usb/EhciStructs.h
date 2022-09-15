#ifndef EHCI_STRUCT_H
#define EHCI_STRUCT_H

#include <stdint.h>

namespace USB {
    // Ehci --------
    struct EhciStructuralParams {
        uint8_t NumPorts:4;
        uint8_t PortPowerCtl:1;
        uint8_t reserved0:2;
        uint8_t PortRouting:1;
        uint8_t PortsPerCompanion:4;
        uint8_t NumCompanionControllers:4;
        uint8_t PortIndicator:1;
        uint8_t reserved1:3;
        uint8_t DebugPort:4;
        uint8_t reserved2;
    } __attribute__((packed));

    struct EhciCapabilityParams {
        uint8_t Addr64:1;
        uint8_t FrameList1024:1;
        uint8_t reserved0:2;
        uint8_t Isochronous_Scheduling_Threshold:4;
        uint32_t reserved1:24;
    };


    // USBSTS
    #define USBSTS_USBINT (1 << 0)
    #define USBSTS_USBERRINT (1 << 1)
    #define USBSTS_PORT_CHANGE_DETECT (1 << 2)
    #define USBSTS_FRAME_LIST_ROLLOVER (1 << 3)
    #define USBSTS_HOST_SYSTEM_ERROR (1 << 4)
    #define USBSTS_INERRUPT_ON_ASYNC_ADVANCE (1 << 5)
    #define USBSTS_HCHALTED (1 << 12)
    #define USBSTS_RECLAMATION (1 << 13)
    #define USBSTS_PERIODIC_SCHEDULE_STATUS (1 << 14)
    #define USBSTS_ASYNCHRONOUS_SCHEDULE_STATUS (1 << 15)

    struct PortSC {
        uint8_t CurrentConnectStatus:1;
        uint8_t ConnectStatusChange:1;
        uint8_t PortEnabled:1;
        uint8_t PortEnableChange:1;
        uint8_t OvercurrentActive:1;
        uint8_t OvercurrentChange:1;
        uint8_t ForcePortResume:1;
        uint8_t Suspend:1;
        uint8_t PortReset:1;
        uint8_t reserved1:1;
        uint8_t LineStatus:2;
        uint8_t PortPower:1;
        uint8_t PortOwner:1;
        uint8_t PortIndicator:2;
        uint8_t PortTestControl:4;
        uint8_t WakeOnConnectEnable:1;
        uint8_t WakeOnDisconnectEnable:1;
        uint8_t WakeOnOvercurrentEnable:1;
        uint16_t reserved2:9;
    } __attribute__((packed));

    struct EhciTransferDescriptor {
        volatile uint32_t link;
        volatile uint32_t altLink;
        volatile uint32_t token;
        volatile uint32_t buffer[5];
        volatile uint32_t extBuffer[5];

        // internal fields
        uint32_t tdNext;
        uint32_t active;
        uint8_t pad[4];
    } __attribute__((packed));

    struct Link {
        struct Link *prev;
        struct Link *next;
    };

    // ----------------------------------
    // USB Endpoint

    // p.74 
    struct UsbEndpoint //TODO:: change to packed following specification
    {
        uint8_t len;
        uint8_t type;
        uint8_t addr;
        uint8_t attributes;
        uint16_t maxPacketSize;
        uint8_t interval;
        uint8_t toggle:1;
    } __attribute__((packed));

    // USB Device Request

    struct UsbDevReq //TODO:: update
    {
        uint8_t type;
        uint8_t req;
        uint16_t value;
        uint16_t index;
        uint16_t len;
    } __attribute__((packed));

    // ---------------------------------
    // USB Transfer

    typedef struct UsbTransfer //TODO:: update
    {
        UsbEndpoint *endp;
        UsbDevReq *req;
        void *data;
        uint16_t len;
        bool complete;
        bool success;
    } UsbTransfer;


    struct EhciQueueHead {
        uint32_t qhlp;       // Queue Head Horizontal Link Pointer
        uint32_t ch;         // Endpoint Characteristics
        uint32_t caps;       // Endpoint Capabilities
        volatile uint32_t curLink;

        // matches a transfer descriptor
        volatile uint32_t nextLink;
        volatile uint32_t altLink;
        volatile uint32_t token;
        volatile uint32_t buffer[5];
        volatile uint32_t extBuffer[5];

        // internal fields
        UsbTransfer *transfer;
        Link qhLink;
        uint32_t tdHead;
        uint32_t active;
        uint8_t pad[20];
    };

    // mindshare whitepaper p.16
    struct EhciCapbilites {
        uint8_t length;
        uint8_t reserved;
        uint16_t version;
        EhciStructuralParams structuralParams;
        EhciCapabilityParams capabilityParams;
        uint64_t CompanionPortRoutes:60;
    } __attribute__((packed));
    
    struct EhciOperations {
        uint32_t UsbCmd;
        uint32_t UsbSts;
        uint32_t UsbIntr;
        uint32_t FrameIndex;
        uint32_t CtlDataSegement;   // CTRLDSSEGMENT
        uint32_t PeriodicList;
        uint32_t NextAsyncList;
        uint32_t reserv[9];
        uint32_t ConfigFlag;
        PortSC PortStsCtl;
    } __attribute__((packed));
}

#endif