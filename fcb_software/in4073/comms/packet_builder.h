#ifndef _PACKET_BUILDER_H
#define _PACKET_BUILDER_H

#include "packet.h"


int fpacketf(struct Packet *packet, m_format format, ...);
int fread_packetf(struct Packet *packet, m_format format, ...);

#endif // _PACKET_BUILDER_H