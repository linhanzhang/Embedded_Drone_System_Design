#include "sensors.h"

#include <stdio.h>
#include "gpio.h"
#include "nrf_gpio.h"

#include <inttypes.h>
#include <string.h>
#include "mpu6050/mpu6050.h"
#include "adc.h"
#include "barometer.h"
#include "scheduler.h"
#include "fail.h"
#include "state.h"

#include "control.h"

extern struct state state;

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define BOUND(X, Y, Z) (MAX((X), MIN((Y), (Z))))

int16_t fphi, ftheta, fpsi;
int16_t fsp, fsq, fsr;
int16_t fsax, fsay, fsaz;
uint16_t fbat_volt, ftemperature, fpressure;

int dmp_mode = 1;
static bool filters_enabled = false;


/**
 * @brief initialize sensors
 * 
 * @author Koen Wosten
 */
void sensors_init(void)
{
    dmp_mode = 1;
    filters_enabled = false;
}

/**
 * @brief setup raw mode
 * 
 * @author Koen Wosten
 */
void sensors_enter_raw()
{
    dmp_mode = 0;
    filters_enabled = false;

    imu_init(false, 200);
    scheduler_reset();
    task *task_sensors = scheduler_get_task("sensors");
    if (task_sensors)
    {
        task_sensors->period = 5;
    }
    else
    {
        error_send("TASK NOT FOUND", 15);
    }
}

/**
 * @brief setup dmp mode
 * 
 * @author Koen Wosten
 */
void sensors_enter_dmp()
{
    dmp_mode = 1;
    filters_enabled = false;

    imu_init(true, 100);
    scheduler_reset();
    task *task_sensors = scheduler_get_task("sensors");
    task_sensors->period = 10;
}

/**
 * @brief Enables the filters
 * 
 * @author Koen Wosten
 * 
 * @param sp_offset 
 * @param sq_offset 
 */
void sensors_filtering_start(int32_t sp_offset, int32_t sq_offset){
    filters_enabled = true;
    Butterworth_init();
    KF_init(sp_offset, sq_offset);
}

/**
 * @brief updates the sensor values
 * 
 * @author Hanhzang Lin
 * 
 */
void raw_timer_handler()
{
    if (check_sensor_int_flag())
    {
        nrf_gpio_pin_toggle(RED);
        // char str[20];
        // sprintf(str, "%d", sensor_fifo_count);

        // error_send(str, 20);
        get_sensor_data();
        adc_request_sample();
        read_baro();
    }

    if (!dmp_mode && filters_enabled)
    {

    int32_t buttered_sp = Butterworth_realize(1,((int32_t)sp) << 14);
    int32_t buttered_sq = Butterworth_realize(2,((int32_t)sq) << 14);
    int32_t buttered_sr = Butterworth_realize(0,((int32_t)sr) << 14);
    int32_t buttered_sax = Butterworth_realize(3,((int32_t)sax) << 14);
    int32_t buttered_say = Butterworth_realize(4,((int32_t)say) << 14);
    int32_t buttered_saz = Butterworth_realize(5,((int32_t)saz) << 14);

    KF_realize(buttered_sp,buttered_say,buttered_sq,buttered_sax);

    fsax = buttered_sax >> 14; 
    fsay = buttered_say >> 14;
    fsaz = buttered_saz >> 14;

    fsp = fil_p >> 14;
    fsq = fil_q >> 14;
    fsr = buttered_sr >> 14;

    fphi = fil_phi >> 14;
    ftheta = fil_theta  >> 14;

    // state.ae[0] = sp;
    // state.ae[1] = sr;
    // state.ae[2] = say;
    // state.ae[3] = sax;

    }
    else
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
    }

    fbat_volt = bat_volt;
    ftemperature = temperature;
    fpressure = pressure;
}
