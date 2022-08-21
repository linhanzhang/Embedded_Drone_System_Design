/*------------------------------------------------------------------
 *  mock_mpu6050.c -- give random values for unit testing
 *
 *  Koen Wösten
 *  
 * 6 june 2022
 *------------------------------------------------------------------
 */
#include "../../mpu6050/mpu6050.h"
#include <stdlib.h>

int16_t phi, theta, psi;
int16_t sp, sq, sr;
int16_t sax, say, saz;
uint8_t sensor_fifo_count;

extern bool sensor_mode;	// Sensor_mode = true = dmp, =false = raw mode.

void imu_init(bool __attribute__ ((unused)) dmp, uint16_t __attribute__ ((unused)) interrupt_frequency){
 phi = theta = psi = 0;
 sp = sq = sr = 0;
 sax = say = saz = 0;
 sensor_fifo_count = 0;
}

void get_sensor_data(void){
    phi += rand()%512 - 256;
    theta += rand()%512  - 256;
    psi += rand()%512  - 256;
    sp += rand()%512  - 256;
    sq += rand()%512  - 256;
    sr += rand()%512  - 256;
    sax += rand()%512  - 256;
    say += rand()%512  - 256;
    saz += rand()%512  - 256;
    return;
}

bool check_sensor_int_flag(void){
    return true;
}
void clear_sensor_int_flag(void){
    return;
}
