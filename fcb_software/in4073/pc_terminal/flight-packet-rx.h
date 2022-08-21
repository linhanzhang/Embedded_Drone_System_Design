#ifndef FLIGHT_PACKET_RX_H_
#define FLIGHT_PACKET_RX_H_

// The whole lenth of flight packet FLIGHT_LEN = 5 + 36 = 41
#define FLIGHT_LEN 43
#define FLIGHT_PAYLOAD_LEN FLIGHT_LEN-5

#include <inttypes.h>

// extern uint16_t bat_volt_rx, temperature_rx, pressure_rx;


extern uint8_t magic_rx;    //packet start marker
extern uint8_t len_rx;      //length in bytes of payload
extern uint8_t seq_rx;      //Packet sequence number
extern uint8_t mstype_rx;   // Message type

// received drone data and can be used by further processes
extern uint8_t  mode_rx;
extern uint8_t  dmp_rx;
extern int16_t ae_rx[4];
extern int16_t phi_rx, theta_rx, psi_rx;
extern int16_t sp_rx, sq_rx, sr_rx;
extern int16_t sax_rx, say_rx, saz_rx; 
extern uint8_t FULL_P1_RX;
extern uint8_t FULL_P2_RX;
extern uint8_t YAW_K_RX;
extern int16_t yaw_setpoint_rx;
extern int16_t bat_volt_rx;
extern uint32_t time_stamp_rx;

extern


void flight_packet_parse(uint8_t c);
void log_file_write();
void log_file_init();

#endif