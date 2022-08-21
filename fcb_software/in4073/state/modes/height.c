#include <stdio.h>
#include "../state.h"
#include "../modes.h"
#include "../../control.h"

/*------------------------------------------------------------------
 *  height.c -- height-control mode, P control for yaw and cascaded-P for roll and pitch
 *  height-control mode is realize based on full mode.
 * 
 *  Tianrui Mao
 *  
 *  10 June 2022
 *------------------------------------------------------------------
 */

//#define FULL_DEBUG;

#include <stdio.h>
#include <stdlib.h>

#include "../state.h"
#include "../modes.h"
#include "../../control.h"
#include "fail.h"

void height_init(struct state *state);
void height_run(struct state *state);
void height_exit(struct state *state);


mode height = {
    height_init,
    height_run,
    height_exit
};



#define FULL_DEBUG

void height_init(struct state *state){
        motors_enable();
        pid_init();
}

void height_run(struct state *state)
{
    // TODO only enable motors after inspected by TA
    struct Pid pid;
    pid.Kp = FULL_P1;
    pid.Kp_cascaded = FULL_P2;
    pid.Kd = HEIGHT_P;
   // pid_init();

    // yaw
    int yaw_setpoint = BOUND(-4094, state->js_yaw/8, 4094);
    int yaw_error = yaw_setpoint - state->sr;
    int16_t new_yaw = YAW_P * yaw_error;

    
    // roll
    int roll_setpoint = BOUND(-4094, state->js_roll / 8, 4094);
    int16_t new_roll = cascaded_p_control(roll_setpoint, state->sp, state->phi, pid);
    

    // pitch
    int pitch_setpoint = BOUND(-4094, state->js_pitch / 8 , 4094);
    int16_t new_pitch = cascaded_p_control(pitch_setpoint, state->sq, state->theta,pid);

    // lift 
    // use pd control to make the loop more stable, P control is also ok.
    int lift_setpoint = BOUND(-4094, state->js_lift / 8, 4094);
    //int fil_pressure = Butterworth_realize_height(state->pressure);
    int16_t new_lift = pd_control(lift_setpoint, state->pressure, pid);

    // #ifdef HEIGHT_DEBUG
    // // use sax/say/saz to print new_yaw/new_roll/new_pitch
    // state->sax = new_yaw;
    // state->say = new_roll;
    // state->saz = new_pitch;

    // #endif

    motors_set_momentum(new_roll, new_pitch, new_yaw, new_lift);

    update_motors();

}

void height_exit(struct state *state){
        
}