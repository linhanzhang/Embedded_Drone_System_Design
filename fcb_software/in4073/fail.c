#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>
#include "comms/packet.h"
#include "comms/packet_builder.h"
#include "comms/packet_tx.h"
#include "hal/uart.h"

/**
 * @brief Function that sends an error message to the PC
 *
 * @author Koen Wosten
 *
 * @param msg the message
 * @param n number of bytes of the message
 */
void error_send(char *msg, uint8_t n)
{
	struct Packet *packet = malloc(sizeof(struct Packet));

	if (packet == NULL)
	{
		return;
	}

	int err = fpacketf(packet, MSG_ERROR(n), msg);

	if (!err)
	{
		send_packet(packet, uart_put);
		packet_free(packet);
	}
}