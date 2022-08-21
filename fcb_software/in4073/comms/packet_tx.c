

/*------------------------------------------------------------
 * packet_tx.c -> Transmits packet over serial
 *
 * Author - koen Wosten
 *
 * 28 May 2022
 *------------------------------------------------------------
 */

#include "packet.h"
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

int seq_tx = 0;

/**
 * @brief sends a packet
 * 
 * @author Hanzhang Lin
 * 
 * @param packet 
 * @param out_func 
 */
void send_packet(struct Packet *packet, void* out_func(uint8_t) )
{
	packet->seq = seq_tx++;
	uint8_t *serialized_packet = packet_serialize(packet); 

	int i = 0;

	while ( i < packet->len + 5)
	{
		out_func(serialized_packet[i]);
		i++;
	}

	free(serialized_packet);
}

