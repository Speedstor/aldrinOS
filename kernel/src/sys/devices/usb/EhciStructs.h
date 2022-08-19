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
        uint32_t ConfigFlags;
        uint32_t PortStsCtl;
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
}

#endif