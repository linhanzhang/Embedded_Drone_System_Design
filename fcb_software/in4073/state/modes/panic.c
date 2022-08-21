/*------------------------------------------------------------------
 *  panic.c -- panic mode, drone deaccelerates its rotors till stop
 *
 *  Koen Wösten
 *  
 *  18 May 2022
 *------------------------------------------------------------------
 */
#include <stdio.h>

#include "nrf_gpio.h"
#include "../../hal/gpio.h"

#include "../state.h"
#include "../modes.h"
#include "../../control.h"

#include "timers.h"

#define TIME_PANIC 2000 //Time taken for the motor to completely de-energize in msec

#define COUNT TIME_PANIC/TIMER_PERIOD //Time in numbers of state updates.


void panic_init(struct state *state);
void panic_run(struct state *state);
void panic_exit(struct state *state);

mode panic = {
    panic_init,
    panic_run,
    panic_exit
};

/**
 * @brief initialization function for the panic state
 * 
 * - sets red light
 * - enables motors
 * 
 * @param state 
 *
 * @author Koen Wosten 
 */
void panic_init(struct state *state){
    nrf_gpio_pin_clear(RED);
    motors_enable();
}

/**
 * @brief running function for the panic state
 * 
 * - slowly de-accelerates the motors.
 * @param state 
 * 
 * @author Koen Wosten 
 */
void panic_run(struct state *state)
{
    static int count = COUNT;

    motors_enable();

    //printf("Entered state: %s\n", __func__); // print current function    

    if ((state->ae[0]>0 || state->ae[1]>0 || state->ae[2]>0 || state->ae[3]>0) && count) 
    {
            state->ae[0] = state->ae[0] - (state->ae[0]/count);
            state->ae[1] = state->ae[1] - (state->ae[1]/count);
            state->ae[2] = state->ae[2] - (state->ae[2]/count);
            state->ae[3] = state->ae[3] - (state->ae[3]/count);
    
        count--;
    }

    if (motor[0]==0 && motor[1]==0 && motor[2]==0 && motor[3]==0 && state->ae[0]==0 && state->ae[1]==0 && state->ae[2]==0 && state->ae[3]==0){
        //enter safe mode
        state_request_mode(MODE_SAFE);
        count = COUNT;
    }

	update_motors();

}

/**
 * @brief exit function for the panic state
 * 
 * - disables motors
 * - turns off red LED
 * 
 * @param state
 * 
 * @author Koen Wosten 
 */
void panic_exit(struct state *state){
    motors_disable();
    nrf_gpio_pin_set(RED);
}
