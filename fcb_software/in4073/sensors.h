/*------------------------------------------------------------------
 *  sensors.h -- Prototypes for the sensor
 *
 *  Koen Wösten
 *  
 *  8 June 2022
 *------------------------------------------------------------------
 */

#ifndef SENSORS_H_
#define SENSORS_H_

#include <inttypes.h>

extern int16_t fphi, ftheta, fpsi;
extern int16_t fsp, fsq, fsr;
extern int16_t fsax, fsay, fsaz;
extern uint16_t fbat_volt, ftemperature, fpressure;

extern int dmp_mode;

void sensors_init(void);
void sensors_filtering_start(int32_t sp_offset, int32_t sq_offset);
void sensors_enter_raw(void);
void sensors_enter_dmp(void);
void raw_timer_handler();

#endif // SENSORS_H_