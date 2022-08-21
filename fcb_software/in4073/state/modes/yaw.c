/*------------------------------------------------------------------
 *  yaw.c -- yaw mode
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

void yaw_init(struct state *state);
void yaw_run(struct state *state);
void yaw_exit(struct state *state);

mode yaw = {
    yaw_init,
    yaw_run,
    yaw_exit
};

void yaw_init(struct state *state){
    motors_enable();
}


// richardfang updated yaw_setpoint_offset controlled by keyboard(q & w)
void yaw_run(struct state *state)
{
    // TODO only enable motors after inspected by TA
    motors_enable();

	//16.4 LSB/deg/s (+-2000 deg/s) (max range of the sr, no real use in joystick doing so much, so scaling it
    // int e = state->js_yaw/32 - (state->sr); // 32 gives us max ~62.5 degree / s

    // introduce yaw_setpoint for inspecting and can be switched by keyboard
    //if (state->js_yaw ==0) yaw_setpoint_offset = 0;
    //TODO Test if 62.5 degree is enough
    int yaw_setpoint = BOUND(-4095, state->js_yaw/8, 4095);
    int error = yaw_setpoint - state->sr;

    int16_t new_yaw = YAW_P * error;

    //printf("Entered state: %s\n", __func__); // print current function
    int16_t moments[3];
    motors_calc_moments(moments, state->js_roll >> 3, state->js_pitch >> 3 , new_yaw);
    motors_set_momentum(moments[0], moments[1], moments[2], state->js_lift);

    update_motors();
}

void yaw_exit(struct state *state){
    
}

