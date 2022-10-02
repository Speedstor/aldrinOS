#ifndef EHCI_PACKET_DEF
#define EHCI_PACKET_DEF

#include <stdint.h>

namespace USB {
    #define QTD_ERR_COUNT 0b11
    #define EMPTY_LENGTH 0
    #define DEFAULT_MAX_PACKET_LENGTH 0x40

    // setup packets
    #define SETUP_PACKET_LENGTH 8
    #define SETUP_STATUS 0b10000000
    uint64_t GET_DESCRIPTOR_DATA_BUFFER = 0x0000400080060001;



    // PIDs
    #define PID_OUT 0b00
    #define PID_IN    0b01
    #define PID_SETUP 0b10
}

#endif