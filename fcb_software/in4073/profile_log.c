/*------------------------------------------------------------
 *profile_log.c - Profiling and Logging for Drone

 * Author - Siyuan Fang
 *
 * May 2022
 *------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include "profile_log.h"
#include "hal/spi_flash.h"
#include "hal/timers.h"
#include "state/state.h"
#include "in4073.h"

extern struct state state;


bool datalog_enable = false;
uint32_t flash_index = 0;
uint32_t read_index = 0;
FILE *f;

// TODO write log file (real-time)
void log_init(){
    if(flash_index == 0){
        flash_chip_erase();
    }
    f = fopen("drone.log", "a+"); 
    // if (f == NULL) { /* Something is wrong   */}
    fprintf(f, "Group 16 Start Testing ... ...\n");
}



bool log_write(void) {
    // erase memory
    if(flash_index == 0){
        flash_chip_erase();
    }
    /*LOGGING
    | #Bytes 	|       4      |    1     |    2    |     2    	| 2     	| 2   	    | 2   	| 2         | 2   	| 2   	| 2   	| 2   	| 2   	|     2    | 2   	 |     
    |--------	| ---------    | ------  | -------	|-----	    |-------	|-------	|------ |-------	|-----	|-----  |-----	|-----	|-----	|-----	   |-----	 |
    | Name   	|ES system time| mode    | ae[0] 	| ae[1]  	| ae[2] 	| ae[3]     | phi 	| theta 	| psi   | sp 	| sq 	| sr 	| sax   |    say   |   saz 	 | 
    */
    uint32_t log_time = get_time_us();
    uint8_t log_payload_uint8_t[LOGGING_SIZE];
    int log_payload_int[15] = {log_time, state.next, 
                            2*state.ae[0], 2*state.ae[1], 2*state.ae[2], 2*state.ae[3], 
                            // yaw_setpoint, state.js_pitch, state.js_yaw, state.js_lift,
                            state.phi, state.theta, state.psi,
                            state.sp, state.sq, state.sr,
                            state.sax, state.say, state.saz,
                            };


    log_payload_uint8_t[0] = (log_payload_int[0] >> 24) & 0xFF;
    log_payload_uint8_t[1] = (log_payload_int[0] >> 16) & 0xFF;
    log_payload_uint8_t[2] = (log_payload_int[0] >> 8) & 0xFF;
    log_payload_uint8_t[3] = log_payload_int[0] & 0xFF;

	// MODE BYTE *1
    log_payload_uint8_t[4] = log_payload_int[1];

	// SENSOR BYTE 2*13
	for (int i=0;i <13; i++) {
		log_payload_uint8_t[5+2*i] = FIRSTBYTE(log_payload_int[i+2]);
		log_payload_uint8_t[5+2*i + 1] = SECONDBYTE(log_payload_int[i+2]);
	}

    if(flash_write_bytes(flash_index, log_payload_uint8_t, LOGGING_SIZE)){
        flash_index += LOGGING_SIZE;
    }
    else{
        return false;
    }
    return true;
}

void log_read(void){
    uint8_t buffer[LOGGING_SIZE] = {0};
    read_index = flash_index - LOGGING_SIZE;
    
    if(flash_read_bytes(read_index, buffer,LOGGING_SIZE)){
        uint32_t time_stamp = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
        fprintf(f, "%10ld ", time_stamp);
        for (int i = 4; i<LOGGING_SIZE;i++){
            fprintf(f, "%X ", buffer[i]);
        }
        fprintf(f, "\n");
    }
    //TODO check index
    read_index = 0;   
} 


// void profile(){
//     ;
// }
// void state_step(void)
// {
//     static uint32_t last_arrival = 0;
// 	static uint32_t delta_time = 0;
//     state_next_mode();
//     delta_time = get_time_us() - last_arrival;
//     Sensor_delta_time = delta_time;
//     state_read_sensors();
//     last_arrival = get_time_us();;