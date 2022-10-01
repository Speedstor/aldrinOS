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


    // ------------------------------------------------------------------------------------------------
    // Port Status and Control Registers
    #define EHCI_PORTSC_OFFSET                   0x44
    #define EHCI_PORT_CONNECTION                 (1 << 0)    // Current Connect Status
    #define EHCI_PORT_CONNECTION_CHANGE          (1 << 1)    // Connect Status Change
    #define EHCI_PORT_ENABLE                     (1 << 2)    // Port Enabled
    #define EHCI_PORT_ENABLE_CHANGE              (1 << 3)    // Port Enable Change
    #define EHCI_PORT_OVER_CURRENT               (1 << 4)    // Over-current Active
    #define EHCI_PORT_OVER_CURRENT_CHANGE        (1 << 5)    // Over-current Change
    #define EHCI_PORT_FORCE_PORT_RESUME          (1 << 6)    // Force Port Resume
    #define EHCI_PORT_SUSPEND                    (1 << 7)    // Suspend
    #define EHCI_PORT_RESET                      (1 << 8)    // Port Reset
    #define EHCI_PORT_LS_MASK                    (3 << 10)   // Line Status
    #define EHCI_PORT_LS_SHIFT                   10
    #define EHCI_PORT_POWER                      (1 << 12)   // Port Power
    #define EHCI_PORT_OWNER                      (1 << 13)   // Port Owner
    #define EHCI_PORT_IC_MASK                    (3 << 14)   // Port Indicator Control
    #define EHCI_PORT_IC_SHIFT                   14
    #define EHCI_PORT_TC_MASK                    (15 << 16)  // Port Test Control
    #define EHCI_PORT_TC_SHIFT                   16
    #define EHCI_PORT_WKCNNT_E                   (1 << 20)   // Wake on Connect Enable
    #define EHCI_PORT_WKDSCNNT_E                 (1 << 21)   // Wake on Disconnect Enable
    #define EHCI_PORT_WKOC_E                     (1 << 22)   // Wake on Over-current Enable
    #define EHCI_PORT_RWC                        (PORT_CONNECTION_CHANGE | PORT_ENABLE_CHANGE | PORT_OVER_CURRENT_CHANGE)

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


    // should be 32 byte in size to be 32 byte aligned (32 bit is 4 bytes)
    struct QH {
        uint32_t qhlp;       // Queue Head Horizontal Link Pointer
        uint32_t ch;         // Endpoint Characteristics
        uint32_t caps;       // Endpoint Capabilities
        volatile uint32_t curLink;
        // --- 16 bytes

        // matches a transfer descriptor
        volatile uint32_t nextLink;
        volatile uint32_t altLink;
        volatile uint32_t token;
        volatile uint32_t buffer[5];
        // --- 48 bytes

        // custom software fields
        uint32_t field1;
        uint32_t field2;
        uint32_t field3;
        uint32_t field4;
        // --- 64 bytes
    };

    //should be 32bytes to align with 32bytes
    struct qTD {
        volatile uint32_t link;
        volatile uint32_t altLink;
        volatile uint32_t token;
        volatile uint32_t pBuffer[5];
        // --- 32 bytes
    } __attribute__((packed));

    #define QTD_NEXT_TERMINATE (1 << 0)

    #define QTD_TOKEN_DATA_TOGGLE (1 << 31)
    #define QTD_TOKEN_LENGTH_SHIFT 16
    #define QTD_TOKEN_IOC (1 << 15)
    #define QTD_TOKEN_CURRENT_PAGE_SHIFT 12
    #define QTD_TOKEN_ERROR_COUNTER_SHIFT 10
    #define QTD_TOKEN_PID_SHIFT 8
    #define QTD_TOKEN_STATUS_ACTIVE (1 << 7)
    #define QTD_TOKEN_STATUS_HALTED (1 << 6)
    #define QTD_TOKEN_STATUS_BUFFER_ERROR (1 << 5)
    #define QTD_TOKEN_STATUS_BABBLE_DETECTED (1 << 4)
    #define QTD_TOKEN_STATUS_TRANSACTION_ERROR (1 << 3)
    #define QTD_TOKEN_STATUS_MISSED_MFRAME (1 << 2)
    #define QTD_TOKEN_STATUS_SPLIT_TRANSACTION_STATUS (1 << 1)
    #define QTD_TOKEN_STATUS_PING (1 << 0)

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
        uint32_t PortStsCtl;
    } __attribute__((packed));
}

#endif