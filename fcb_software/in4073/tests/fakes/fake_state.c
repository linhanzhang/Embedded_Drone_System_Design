
/*------------------------------------------------------------------
 *  fake_state.h -- minimal fake of the state for unit testing
 *
 *  Koen Wösten
 *  
 *  24 May 2022
 *------------------------------------------------------------------
 */

#include "../../state/state.h"

struct state state;
void state_init(void)
{
    state = (struct state) {
            1, 1, false, false,
            {0,0,0,0}, // Offset
            0,0,0,0, // Yaw, Pitch, Roll, Lift
            {0,0,0,0}, // AE 
            0,0,0,     //phi, theta, psi;
            0,0,0,     // sp, sq, sr;
            0,0,0,     // sax, say, saz;
            0,0,0,     // bat_volt, temperature, pressure;
            0,0,0,0,0,0,0,0,0 }; //sensor offsets
}



void state_read_sensors(void){
    return;
}

void state_request_mode(__attribute__ ((unused)) int requested){
    return;
}


void state_request_abort(void){
    return;
}

void state_next_mode(void)
{
    return;
}

void state_step(void)
{
    return;
}

void state_set_jsdata(__attribute__ ((unused)) uint16_t roll, __attribute__ ((unused)) uint16_t pitch, __attribute__ ((unused)) uint16_t yaw, __attribute__ ((unused)) uint16_t lift)
{
    return;
}


void state_set_offset(__attribute__ ((unused)) int index, __attribute__ ((unused)) int16_t offset)
{
    return;
}


int state_get_mode(void){
    return 1;
}