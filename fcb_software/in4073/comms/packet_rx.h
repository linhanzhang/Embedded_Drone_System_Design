#ifndef _PACKET_RX_H
#define _PACKET_RX_H

#include <inttypes.h>
#include "packet.h"
 
#define ADDITION_BYTE 5

int receive_packet(struct Packet **packet, uint8_t in_func(), int* condition);

#endif // _PACKET_RX_H