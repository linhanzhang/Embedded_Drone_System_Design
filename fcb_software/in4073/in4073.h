/*------------------------------------------------------------------
 *  in4073.h -- defines, globals, function prototypes
 *
 *  I. Protonotarios
 *  Embedded Software Lab
 *
 *  July 2016
 *------------------------------------------------------------------
 */
#ifndef IN4073_H__
#define IN4073_H__

#include "comms/packet.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "hal/spi_flash.h"
#include "hal/timers.h"
#include "control_packet_rx.h"
#include "profile_log.h"

//#include "control.h"


// #define POLY 0xe7
// #define DEFAULT_PACKET_LEN 4
// #define DEFAULT_PACKET_TYPE 1 // 
// #define ADDITION_BYTE 5
// #define CONTROL_PACKET_LENGTH 9

// uint8_t index_c = 0;
// uint8_t packet[DEFAULT_PACKET_LEN+5]={0};
// uint8_t payload_length = DEFAULT_PACKET_LEN;
// uint8_t packet_type = DEFAULT_PACKET_TYPE;
// uint8_t crc;





extern bool demo_done;


// divide uint16_t into two bytes
#define FIRSTBYTE(x) ((uint8_t) ((x & 0xff00) >> 8))
#define SECONDBYTE(x) ((uint8_t) ((x & 0x00ff)))

// function for flight-packet-tx
void uart_put(uint8_t);
void send_flight_data(struct Packet *packet);
void flight_packet_update(struct Packet *packet);

#endif // IN4073_H__
