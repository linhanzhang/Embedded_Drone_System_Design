/*------------------------------------------------------------------
 *  full.c -- full-control mode, P control for yaw and cascaded-P for roll and pitch
 *  full-control mode is realize based on yaw mode.
 * 
 *  Tianrui Mao
 *  
 *  18 May 2022
 *------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>

#include "../state.h"
#include "../modes.h"
#include "../../control.h"
#include "fail.h"

void full_init(struct state *state);
void full_run(struct state *state);
void full_exit(struct state *state);


mode full = {
    full_init,
    full_run,
    full_exit
};



#define FULL_DEBUG

void full_init(struct state *state){
        motors_enable();
}

void full_run(struct state *state)
{
    // TODO only enable motors after inspected by TA
    motors_enable();

    struct Pid pid;
    pid.Kp = FULL_P1;
    pid.Kp_cascaded = FULL_P2;
   // pid_init();

    // yaw
    int yaw_setpoint = BOUND(-4094, state->js_yaw / 8, 4094);
    int yaw_error = yaw_setpoint - state->sr;
    int16_t new_yaw = YAW_P * yaw_error;

    
    // roll
    int roll_setpoint = BOUND(-4094, state->js_roll / 8, 4094);
    int16_t new_roll = cascaded_p_control(roll_setpoint, state->sp, state->phi, pid);
    

    // pitch
    int pitch_setpoint = BOUND(-4094, state->js_pitch / 8, 4094);
    int16_t new_pitch = cascaded_p_control(pitch_setpoint, state->sq, state->theta,pid);

    #ifdef DEBUG
        printf("[error outer loop] %d \n",pid.err);
        printf("[error inner loop] %d \n",pid.err_cascaded);

    #endif

    int16_t moments[3];
    motors_calc_moments(moments, new_roll, -new_pitch, new_yaw);
    motors_set_momentum(moments[0], moments[1], moments[2], state->js_lift);

    update_motors();

}

void full_exit(struct state *state){
        
}