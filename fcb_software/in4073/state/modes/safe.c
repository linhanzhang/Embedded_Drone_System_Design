/*------------------------------------------------------------------
 *  safe.c -- functions for deadling with different modes
 *
 *  Koen Wösten
 * 
 *  11 May 2022
 *------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>


#include "nrf_gpio.h"
#include "../../hal/gpio.h"

#include "../state.h"
#include "../modes.h"
#include "../../control.h"

void safe_init(struct state *state);
void safe_run(struct state *state);
void safe_exit(struct state *state);

mode safe = {
    safe_init,
    safe_run,
    safe_exit
};

void safe_init(struct state *state){
    nrf_gpio_pin_clear(GREEN);
    motors_disable();
}

void safe_run(struct state *state)
{


    motors_disable();

    state->offset[0] = 0;
    state->offset[1] = 0;
    state->offset[2] = 0;
    state->offset[3] = 0;

    if (state->requested_abort){
        state->requested_abort = false;
        abort();
    }

    //printf("Entered state: %s\n", __func__); // print current function
}

void safe_exit(struct state *state)
{
    motors_disable();
    nrf_gpio_pin_set(GREEN);
}

