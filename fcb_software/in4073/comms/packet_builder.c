
/*------------------------------------------------------------
 * packet_builder.c - Tools to format and destruct packets
 *
 * author - 
 * Koen Wosten - 11 June 2022
 * 
 *------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "packet_builder.h"

/**
 * @brief builds a packet according to format
 * 
 * @param packet 
 * @param format 
 * @param ... list of arguments, order matters, ordered on bytes size, small first.
 * @return int 0 if succes.
 * 
 * @author Koen Wosten
 */
int fpacketf(struct Packet *packet, m_format format, ...){
    va_list valist;
    int i, err = 0;

    va_start(valist, format );

    if (!packet_format_is_valid(format)){
        err = 1;
        goto cleanup;
    }

    packet->mstype = format.x[0];
    packet->len = format.x[1];
    packet->payload = malloc(format.x[1]);

    if(!packet->payload){
        err = 1;
        goto cleanup;
    }

    for(i = 0; i < format.x[2]; i++){
        packet->payload[i] = (uint8_t) va_arg(valist, int);
    }
    for(i = format.x[2]; i < format.x[2] + format.x[3]*2; i+=2){
        uint16_t val = (uint16_t) va_arg(valist, int);
        packet->payload[i]         = FIRSTBYTE(val);
        packet->payload[i + 1]     = SECONDBYTE(val);
    }
    for(; i < format.x[2] + format.x[3]*2 + format.x[4]*4; i+=4){
        uint32_t val = (uint32_t) va_arg(valist, int);
        packet->payload[i]   = (val >> 24) & 0xFF;
        packet->payload[i+1] = (val >> 16) & 0xFF;
        packet->payload[i+2] = (val >> 8) & 0xFF;
        packet->payload[i+3] = val & 0xFF;
    }
    if (format.x[5]){
        memcpy(packet->payload+i, va_arg(valist, int*), format.x[1] - i);
    }

cleanup:
    va_end(valist);

    return err;
}

/**
 * @brief reads a packet according to a format
 * 
 * 
 * @param packet 
 * @param format 
 * @param ... list of arguments, order matters, ordered on bytes size, small first.
 * @return int 
 * 
 * @author Koen Wosten
 */
int fread_packetf(struct Packet *packet, m_format format, ...){
    va_list valist;
    int i = 0;
    int err = 0;

    va_start(valist, format );

    if (!packet_format_is_valid(format)){
        err = 1;
        goto cleanup;
    }

    for(i = 0; i < format.x[2]; i++){
        int *arg = va_arg(valist, int*);
        memcpy(arg, packet->payload+i, 1);
    }

    for(i =  format.x[2]; i < format.x[2] + format.x[3]*2; i+=2){
        int *arg = va_arg(valist, int*);
        uint16_t val = (packet->payload[i] << 8) | packet->payload[i+1];
        memcpy(arg, &val, 2);
    }
    
    for(; i < format.x[2] + format.x[3]*2 + format.x[4]*4; i+=4){
        int *arg = va_arg(valist, int*);
        uint32_t val = (packet->payload[i] << 24) | (packet->payload[i+1] << 16) | (packet->payload[i+2] << 8) | packet->payload[i+3];
        memcpy(arg, &val, 4);
    }

    if(format.x[5]){
        void *arg = va_arg(valist, int*);
        memcpy(arg, packet->payload+i, format.x[1] - i);
    }

cleanup:
    va_end(valist);

    return err;
}