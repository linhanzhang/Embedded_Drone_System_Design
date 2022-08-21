/*------------------------------------------------------------
 * flight-packet-rx.c - Parses and Process the telemetry data received from drone     
 *
 * Author - Siyuan Fang
 *
 * April 2022
 *------------------------------------------------------------
 */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "pc_terminal.h"
#include "../comms/packet.h"
#include "flight-packet-rx.h"
#include "stdbool.h"

// #define Yaw_Mode_DEBUG

uint8_t magic_rx;    //packet start marker
uint8_t len_rx;      //length in bytes of payload
uint8_t seq_rx;      //Packet sequence number
uint8_t mstype_rx;   // Message type

// received drone data and can be used by further processes
uint8_t  mode_rx;
uint8_t dmp_rx;
int16_t ae_rx[4];
int16_t phi_rx, theta_rx, psi_rx;
int16_t sp_rx, sq_rx, sr_rx;
int16_t sax_rx, say_rx, saz_rx;

uint8_t FULL_P1_RX;
uint8_t FULL_P2_RX;
uint32_t time_stamp_rx;
FILE *f;
bool log_flag = true;

static char* mode_strs[8] = {"FAIL", "SAFE", "PANIC", "MANUAL", "CALIBRATE", "YAW", "FULL", "RAW"};
uint8_t YAW_K_RX;
int16_t yaw_setpoint_rx;
int16_t bat_volt_rx;


// siyuan fang :LOG FILE INIT: will generte a log file named "drone_sensor.log"
void log_file_init(){
    char buff[100];
    time_t now = time (0);
    strftime (buff, 100, "%Y-%m-%d %H-%M-%S", localtime (&now));
    
    f = fopen(strcat(buff, ".log"), "a+"); 
    // if (f == NULL) { /* Something is wrong   */}
    fprintf(f, "Group 16 begins Logging ... ...\n");
    log_flag = false;
}

// siyuan fang: both write the flight packet to terminal and log file
void log_file_write(){
    // Print to terminal
    fprintf(stderr, "| %10d ",time_stamp_rx);
    fprintf(stderr, "%10s ", mode_strs[mode_rx]);
    fprintf(stderr, "%3s | ", (dmp_rx ? "DMP" : "RAW"));
    fprintf(stderr, "%6d %6d %6d %6d | ",ae_rx[0], ae_rx[1], ae_rx[2], ae_rx[3]);
    fprintf(stderr, "%6d %6d %6d | ", phi_rx/182, theta_rx/182, psi_rx/182);
    fprintf(stderr, "%6d %6d %6d | ", sp_rx, sq_rx, sr_rx);
    fprintf(stderr, "%6d %6d %6d | ", sax_rx, say_rx, saz_rx);
    fprintf(stderr, "%3d %3d | ", FULL_P1_RX, FULL_P2_RX);
    fprintf(stderr, "%3d %6d | ", YAW_K_RX, yaw_setpoint_rx);
    fprintf(stderr, "%6d | ", bat_volt_rx);

    fprintf(stderr, "\n");

    // Print to log file   
    fprintf(f, "%10d, ",time_stamp_rx);
    fprintf(f, "%10s ", mode_strs[mode_rx]);
    fprintf(f, "%3s | ", (dmp_rx ? "DMP" : "RAW"));
    fprintf(f, "%6d, %6d, %6d, %6d, ",ae_rx[0], ae_rx[1], ae_rx[2], ae_rx[3]);
    fprintf(f, "%6d, %6d, %6d,", phi_rx, theta_rx, psi_rx);
    fprintf(f, "%6d, %6d, %6d, ", sp_rx, sq_rx, sr_rx);
    fprintf(f, "%6d, %6d, %6d ", sax_rx, say_rx, saz_rx);
    fprintf(f, "%3d %3d | ", FULL_P1_RX, FULL_P2_RX);
    fprintf(f, "%3d %6d | ", YAW_K_RX, yaw_setpoint_rx);
    fprintf(f, "%6d | ", bat_volt_rx);
    fprintf(f, "\n");
}

