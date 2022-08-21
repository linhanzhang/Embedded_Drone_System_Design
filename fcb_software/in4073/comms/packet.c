/*------------------------------------------------------------
 * packet.c - Tools to serialize and deserialize packets
 *
 * author - 
 * Koen Wosten 
 * 
 *------------------------------------------------------------
 */

#include <string.h>
#include <stdlib.h>
#include "packet.h"

#include <stdio.h>



#define POLY 0xe7

/**
 * @brief loop-unrolled version of 8-bit CRC Model - sourcer32@gmail.com
 * as adapted from https://community.st.com/s/question/0D50X0000CDmAkpSQF/calculate-crc8
 * 
 * @author sourcer32@gmail.com
 * 
 * @copyright Copyright (C) 1988-2020, All Rights Reserved
 * 
 * @param input 
 * @param n 
 * @return uint8_t 
 */
uint8_t crc8(uint8_t *input, int n){
  uint8_t crc = 0x00;
   
  while(n--)
  {
    crc ^= *input++; // Apply Byte

    crc = (crc << 1) ^ ((crc & 0x80) ? POLY : 0x0);
    crc = (crc << 1) ^ ((crc & 0x80) ? POLY : 0x0);
    crc = (crc << 1) ^ ((crc & 0x80) ? POLY : 0x0);
    crc = (crc << 1) ^ ((crc & 0x80) ? POLY : 0x0);
    crc = (crc << 1) ^ ((crc & 0x80) ? POLY : 0x0);
    crc = (crc << 1) ^ ((crc & 0x80) ? POLY : 0x0);
    crc = (crc << 1) ^ ((crc & 0x80) ? POLY : 0x0);
    crc = (crc << 1) ^ ((crc & 0x80) ? POLY : 0x0);
  }
 
  return(crc);
}

/**
 * @brief serializes a packet
 * 
 * @author Koen Wosten
 * 
 * @param packet 
 * @return uint8_t* 
 */
uint8_t* packet_serialize(struct Packet *packet){
    /*
    | #Bytes 	| 1     	| 1   	| 1   	| 1    | 0-255   	| 1   	|
    |--------	|-------	|-----	|-----	|----- |---------	|-----	|
    | Name   	| MAGIC 	| LEN 	| SEQ 	| TYPE | PAYLOAD 	| CRC 	|
    */

    int length = packet->len + 5; //5 other bytes
    
    uint8_t* packet_out = malloc(length * sizeof *packet_out); 

    packet_out[MAGIC]   = MAGIC_HEADER;
    packet_out[LEN]     = packet->len;
    packet_out[SEQ]     = packet->seq;
    packet_out[MSTYPE]  = packet->mstype;
    memcpy(&packet_out[PAYLOAD], packet->payload, packet->len);
    packet_out[length-1] = crc8(&packet_out[LEN], length-2); //crc without magic and crc

    return packet_out;
}

/**
 * @brief deserializes a packet
 * 
 * @author Koen Wosten
 * 
 * @param data 
 * @return struct Packet* 
 */
struct Packet *packet_deserialize(uint8_t *data){
    struct Packet *packet = malloc(sizeof *packet);
    packet->payload = NULL;
    
    //check start byte
    if(data[MAGIC] != MAGIC_HEADER){
        //needs better error handeling
        //fprintf(stderr, "[%s] %#02x should be %#02x \n", __func__, data[MAGIC], MAGIC_HEADER);
        packet_free(packet);
        return NULL;
    }

    //check crc
    int length = data[LEN] + 5; //5 other bytes
    if(data[length-1] != crc8(&data[LEN], length-2)){
        //fprintf(stderr, "[%s] %#02x should be %#02x \n", __func__, data[length-1], crc8(&data[LEN], length-2) );
        packet_free(packet);
        return NULL;

    }

    packet->len     = data[LEN];
    packet->seq     = data[SEQ];
    packet->mstype  = data[MSTYPE];
    packet->payload = malloc(packet->len * sizeof *packet->payload);
    memcpy(packet->payload, &data[PAYLOAD], packet->len);

    return packet;
}
/**
 * @brief free a packet
 * 
 * @author Koen Wosten
 * 
 * @param packet 
 */
void packet_free(struct Packet *packet){
    free(packet->payload);
    free(packet);
}

/**
 * @brief cheks of a packet format is valid
 * 
 * @author Koen Wosten
 * 
 * @param format 
 * @return int 
 */
int packet_format_is_valid(m_format format){
    if (format.x[2] + format.x[3] * 2 <= format.x[1]){
      return 1;
    } 

    return 0;

}