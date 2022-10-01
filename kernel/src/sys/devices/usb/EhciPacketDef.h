#ifndef EHCI_PACKET_DEF
#define EHCI_PACKET_DEF

#include <stdint.h>

namespace USB {
    #define QTD_ERR_COUNT 0b11

    // setup packets
    #define SETUP_PACKET_LENGTH 8
    #define SETUP_STATUS 0b10000000
    uint64_t GET_DESCRIPTOR_DATA_BUFFER = 0x00080E80;


    // PIDs
    #define PID_SETUP 0b10
}

#endif