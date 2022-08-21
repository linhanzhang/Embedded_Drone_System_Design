/*------------------------------------------------------------------
 *  fake_sensors.c -- minimal fake of the sesnors for unit testing
 *
 *  Koen Wösten
 *  
 *  20 June 2022
 *------------------------------------------------------------------
 */
#include "../sensors.h"

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "../../mpu6050/mpu6050.h"
#include "../../hal/adc.h"
#include "../../hal/barometer.h"

int16_t fphi, ftheta, fpsi;
int16_t fsp, fsq, fsr;
int16_t fsax, fsay, fsaz;
uint16_t fbat_volt, ftemperature, fpressure;

int dmp_mode = 1;

// //TODO: not a fan of this

/**
 * @brief initialize sensors
 * 
 * @author Koen Wosten
 */
void sensors_init(void)
{
    dmp_mode = 1;
}

/**
 * @brief setup raw mode
 * 
 * @author Koen Wosten
 */
void sensors_enter_raw()
{
    dmp_mode = 0;
}

/**
 * @brief setup dmp mode
 * 
 * @author Koen Wosten
 */
void sensors_enter_dmp()
{
    dmp_mode = 1;

}

/**
 * @brief updates the sensor values
 * 
 * @author Koen Wosten
 * 
 */
void raw_timer_handler()
{
    fphi = phi;
    ftheta = theta;
    fpsi = psi;
    fsp = sp;
    fsq = sq;
    fsr = sr;
    fsax = sax;
    fsay = say;
    fsaz = saz;

    fbat_volt = bat_volt;
    ftemperature = temperature;
    fpressure = pressure;
}
