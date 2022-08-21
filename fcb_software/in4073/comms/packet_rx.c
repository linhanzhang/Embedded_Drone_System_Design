

/*------------------------------------------------------------
 * packet_tx.c -> Transmits packet over serial
 *
 * author - 
 * Koen Wosten - 11 June 2022
 * Hanzhang Lin - 28 May 2022
 * 
 *------------------------------------------------------------
 */

#include "packet.h"
#include "packet_rx.h"
#include <inttypes.h>
#include <string.h>
#include <stdio.h>

#define ADDITION_BYTE 5

int process_packet(uint8_t c, struct Packet **packet);

static int seq_rx = 0;

/**
 * @brief receives packet, returns 0 if succesfull and packet can be read read up to 64 bytes, then stops
 * 
 * @author Koen Wosten
 * 
 * @param packet 
 * @param in_func 
 * @param condition 
 * @return int 
 */
int receive_packet(struct Packet **packet, uint8_t in_func(), int* condition )
{
	//TODO: blocking atm
    int count = 0;
	while ( *condition && count++ < 64) {
        unsigned char c = in_func();

        //printf("%3d:", c);

        switch (process_packet(c, packet))
        {
        case -1:
            return 1;

        case 0:
            seq_rx++;
            return 0;

        default:
            ;
        }
    }
    return 1;
}
/**
 * @brief processes the packet
 * 
 * @author Hanzhang Lin
 * 
 * @param c 
 * @param packet 
 * @return int 
 */
int process_packet(uint8_t c, struct Packet **packet){
        /*
    | #Bytes 	| 1     	| 1   	| 1   	| 1    | 0-255   	| 1   	|
    |--------	|-------	|-----	|-----	|----- |---------	|-----	|
    | Name   	| MAGIC 	| LEN 	| SEQ 	| TYPE | PAYLOAD 	| CRC 	|
    */


    static uint8_t index_c = 0;
    static uint8_t packet_buffer[MAX_PACKET_SIZE] = {0};
    static uint8_t payload_len = MAX_PAYLOAD_SIZE;

    //CHECK MAGIC
    if (index_c == 0 && c != MAGIC_HEADER)
    {
        return 1;
    }

    //Save length of the message
    if (index_c == 1)
    {
        payload_len = c;
    }

    //SAVE MESSAGE IN BUFFER
    packet_buffer[index_c] = c;

    //process it
    if (index_c == payload_len + ADDITION_BYTE - 1)
    {
        *packet = packet_deserialize(packet_buffer);

        //reset
        payload_len = 0;
        memset(packet_buffer, 0, sizeof packet_buffer);
        index_c = 0;

        if(!*packet){
            return -1;
        }

        return 0;

    }

    index_c++;
    return 1;

}