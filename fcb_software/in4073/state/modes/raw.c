/*------------------------------------------------------------------
 *  raw.c -- rwa mode
 *
 *  Koen Wösten
 *  Hanzhang Lin
 *  11 May 2022
 *------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>

#include "../state.h"
#include "../modes.h"
#include "../../control.h"
#include "sensors.h"
#include "fail.h"

#define CALIBRATION_TIMES 256

void raw_init(struct state *state);
void raw_run(struct state *state);
void raw_exit(struct state *state);



mode raw = {
    raw_init,
    raw_run,
    raw_exit
};

// Koen Wosten
void raw_init(struct state *state){
    if(dmp_mode){
        sensors_enter_raw();
        state->sp_offset = 0;
        state->sq_offset = 0;
    } else {
        sensors_enter_dmp();
    }
}

 //  Hanzhang Lin
void raw_run(struct state *state)
{
    static uint16_t count = 0;
    static int32_t sp_offset = 0;
    static int32_t sq_offset = 0;

    if(dmp_mode){
        state_request_mode(MODE_SAFE);
        state_next_mode(); //force new mode
    } else {
        if(count++ < CALIBRATION_TIMES){ 
            // Wait until measurements are stable-ish
        } else if(count ++ > CALIBRATION_TIMES){
            sp_offset += state->sp;
            sq_offset += state->sq;

        }
        
        if (count++ > 2*CALIBRATION_TIMES){

            sp_offset = sp_offset / CALIBRATION_TIMES;
            sq_offset = sq_offset / CALIBRATION_TIMES;

            sensors_filtering_start(sp_offset, sq_offset);

            count = 0;

            state_request_mode(MODE_SAFE);
            state_next_mode(); //force new mode
        }
    }
}

// Koen Wosten
void raw_exit(struct state *state){

}

