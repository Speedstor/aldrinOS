#ifndef USB_H
#define USB_H

#include <stdint.h>
#include <stddef.h>
#include "../pci.h"

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

    enum EHCIRegisters {
        USBCMD = 0x00,
        USBSTS = 0x04,
        USBINTR = 0x08,
        FRINDEX = 0x0C,
        CTRLDSSEGMENT = 0x10,
        PERIODICLISTBASE = 0x14,
        ASYNCLISTADDR = 0x18,
        CONFIGFLAG = 0x40,
        PORTSC_BASE = 0x44
    };

    //
    // Define EHCI capability register offsets.
    //

    #define EHCI_CAPABILITY_LENGTH_REGISTER                0x00
    #define EHCI_CAPABILITY_VERSION_REGISTER               0x02
    #define EHCI_CAPABILITY_PARAMETERS_REGISTER            0x04
    #define EHCI_CAPABILITY_PARAMETERS_PORT_COUNT_MASK     0x0000000F
    #define EHCI_CAPABILITY_CAPABILITIES_REGISTER          0x08
    #define EHCI_CAPABILITY_CAPABILITIES_EXTENDED_CAPABILITIES_MASK 0x0000FF00
    #define EHCI_CAPABILITY_CAPABILITIES_EXTENDED_CAPABILITIES_SHIFT 8
    #define EHCI_CAPABILITY_PORT_ROUTING_REGISTER          0x0C

    #define USB_BASE_OFFSET 0x10
    #define PCI_SBRN_OFFSET 0x60
    #define PCI_FLADJ_OFFSET 0x61
    #define PCI_PWAKECAP_OFFSET 0x62

    struct HCStructural {
        uint8_t NumPorts:4;
        uint8_t PortPowerCtl:1;
        uint8_t reserved0:2;
        uint8_t PortRouting:1;
        uint8_t PortsPerCompanion:4;
        uint8_t PortIndicator:1;
        uint8_t reserved1:3;
        uint8_t DebugPort:4;
        uint8_t reserved2;
    } __attribute__((packed));

    struct HCCapability {
        uint8_t Addr64:1;
        uint8_t FrameList1024:1;
        uint8_t reserved0:2;
        uint8_t Isochronous_Scheduling_Threshold;
        uint32_t reserved1:24;
    };



    class UHCIDriver {
    public:
        UHCIDriver(PCI::PCIDeviceHeader* pciBaseAddress);
        ~UHCIDriver();
        PCI::PCIDeviceHeader* pciBaseAddress;
    };

    class EHCIDriver {
        public:
        EHCIDriver(PCI::PCIDeviceHeader* pciBaseAddress);
        ~EHCIDriver();
        PCI::PCIDeviceHeader* pciBaseAddress;
        uint8_t SBRN;
        uint8_t FLADJ;
        uint16_t PWAKECAP;
        uint32_t UsbBase;
        uint32_t UsbOperateBase;
        uint8_t Version;
    };
}


#endif