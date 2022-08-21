/*------------------------------------------------------------------
 *  manual.c -- functions for deadling with different modes
 *
 *  Koen Wösten 
 * 
 *  11 May 2022
 *------------------------------------------------------------------
 */
#include <stdio.h>

#include "../state.h"
#include "../modes.h"
#include "../../control.h"
#include "sensors.h"


void manual_init(struct state *state);
void manual_run(struct state *state);
void manual_exit(struct state *state);

mode manual = {
    manual_init,
    manual_run,
    manual_exit
};

/**
 * @brief initialization function for the manual state
 * 
 * enables the motors
 * 
 * @param state 
 * 
 * @author Koen Wosten
 */
void manual_init(struct state *state){
    motors_enable();

    //printf("Entered state: %s\n", __func__); // print current function
}

/** 
 * @brief running function for the manual state
 * 
 * - enables the motors
 * - maps joystick values to ~ +/-23 degrees
 * - calulates needed moments
 * - sets moments
 * 
 * @param state
 * 
 * @author Koen Wosten 
 */
void manual_run(struct state *state)
{


    motors_enable();

    int16_t moments[3];
    // To get the right angle we asume the joystick is about +/- 23 degrees
    // translate to correct values
    // 65535 * 45/360
    // 45/360 = 8

    motors_calc_moments(moments, state->js_roll >> 3, state->js_pitch >> 3, state->js_yaw);
    motors_set_momentum(moments[0], moments[1], moments[2], state->js_lift);

    update_motors();

}

/**
 * @brief exit function for the manual state
 *  
 * @param state
 * 
 * @author Koen Wosten 
 */
void manual_exit(struct state *state){
    
}