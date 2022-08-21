/*------------------------------------------------------------------
 *  modes.h -- functions for deadling with different modes
 *
 *  Koen Wösten
 *  
 *  11 May 2022
 *------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdbool.h>
#include "modes.h"
#include "state.h"

#define OK true
#define FAIL false

extern struct state state;
//int16_t yaw_setpoint;
//int16_t yaw_setpoint_offset;

void fail_f(struct state *state);

mode fail = {
    fail_f,
    fail_f,
    fail_f
};

void fail_f(struct state *state){
    printf("Entered state: %s\n", __func__); // print current function
}

mode* modes[NUM_MODES] =  {&fail, &safe, &panic, &manual, &calibrate, &yaw, &full, &raw};

bool mode_transitions[NUM_MODES][NUM_MODES] = {
    {FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL},
    {OK, OK, OK, OK, OK, OK, OK, OK},
    {OK, OK, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL}, 
    {OK, OK, OK, FAIL, FAIL, FAIL, FAIL, FAIL},
    {OK, OK, OK, FAIL, FAIL, FAIL, FAIL, OK},
    {OK, OK, OK, FAIL, FAIL ,FAIL, FAIL, FAIL},
    {OK, OK, OK, FAIL, FAIL ,FAIL, FAIL, FAIL},
    {OK, OK, OK, FAIL, OK ,FAIL, FAIL, FAIL}};




