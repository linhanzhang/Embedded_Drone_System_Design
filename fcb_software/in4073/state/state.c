/*------------------------------------------------------------------
 *  state.c -- state and function definitions for a finite state machine.
 *
 *  Koen Wösten
 *  
 *  11 May 2022
 *------------------------------------------------------------------
 */


#include <stdbool.h>
#include <stdio.h>

#include "sensors.h"

#include "state.h"
#include "modes.h"
#include "fail.h"


#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define BOUND(X, Y, Z) (MAX((X), MIN((Y), (Z))))


struct state state;

extern mode *modes[NUM_MODES];
extern bool mode_transitions[NUM_MODES][NUM_MODES];

uint8_t YAW_P = 9;
uint8_t FULL_P1 = 23;
uint8_t FULL_P2 = 19;
uint8_t HEIGHT_P = 8;

bool intialized_state = false;

void state_read_sensors(void);
void state_next_mode(void);

/**
 * @brief initialize the state
 * 
 * @author Koen Wösten
 */
void state_init(void)
{
    if(!intialized_state){
        state = (struct state) {
                MODE_SAFE, MODE_SAFE, false, true, 
                {0,0,0,0}, // Offset
                0,0,0,0, // Yaw, Pitch, Roll, Lift
                {0,0,0,0}, // AE 
                0,0,0,     //phi, theta, psi;
	            0,0,0,     // sp, sq, sr;
                0,0,0,     // sax, say, saz;
	            0,0,0,     // bat_volt, temperature, pressure;
                0,0,0,0,0,0,0,0,0}; //sensor offsets


        
        state_read_sensors();

        intialized_state = true;
    } 
}

/**
 * @brief Get all sensors data and save it in the state.
 * 
 * @author Koen Wösten
 */
void state_read_sensors(void){
        state.phi = fphi - state.phi_offset;
        state.theta = ftheta - state.theta_offset;
        state.psi = fpsi - state.psi_offset;
        state.sp = fsp - state.sp_offset;
        state.sq = fsq - state.sq_offset;
        state.sr = fsr - state.sr_offset;
        state.sax = fsax - state.sax_offset;
        state.say = fsay - state.say_offset;
        state.saz = fsaz - state.saz_offset;
        state.bat_volt = fbat_volt;
        state.temperature = ftemperature;
        state.pressure = fpressure;
}

/**
 * @brief Request the next mode
 * 
 * @param requested 
 * 
 * @author Koen Wösten
 */
void state_request_mode(int requested){
    state.requested = requested;
    char str[15];

    sprintf(str, "Requested %d", state.requested);

    error_send(str,15);
}

/**
 * @brief Request the next mode
 * 
 * @param requested 
 * 
 * @author Koen Wösten
 */
void state_request_abort(){
    if ( state.next == MODE_SAFE || state.next == MODE_PANIC || state.requested == MODE_PANIC || state.requested == MODE_SAFE ){
        state.requested_abort = true;
    }
}

/**
 * @brief Checks if the requested mode is valid, if so sets is as the next state.
 * 
 * @author Koen Wösten
 */
void state_next_mode()
{
    if (state.requested)
    { // So it is not FAIL
        if (mode_transitions[state.next][state.requested]) // only allow back to SAFE/PANIC and to other
        { 
            if (state.next == MODE_SAFE)
            {
                if (state.js_roll || state.js_pitch || state.js_yaw || state.js_lift || state.ae[0] || state.ae[1] || state.ae[2] || state.ae[3])
                {
                    //FAIL
                    char str[46];

					sprintf(str, "Input non-zero: %6d %6d %6d %6d\n", state.js_roll, state.js_pitch, state.js_yaw, state.js_lift);

					error_send(str, 46);
                } else {
                    modes[state.next]->exit(&state);
                    state.next = state.requested;
                    modes[state.next]->init(&state);
                    printf("changed to %d\n", state.requested);
                    

                }
            }
            else
            {
                modes[state.next]->exit(&state);
                state.next = state.requested;
                modes[state.next]->init(&state);
                printf("changed to %d\n", state.requested);

            }

        }
        else
        {
            error_send("transition not allowed", 23);
        }
    }

    state.requested = MODE_FAIL;
}

/**
 * @brief advance the state-machine one step
 * 
 * sets state to the next state, reads the new sensors, and executes the next state.
 * 
 * @author Koen Wösten
 */
void state_step(void)
{
    state_next_mode();
    state_read_sensors();

    if(state.next) {
        modes[state.next]->run(&state);
    }
    else {
        printf("something failed with state transistion\n");
    }
}

/**
 * @brief save the given joystick data and add any needed offset.
 * 
 * @param payload payload containing joystick data
 */
void state_set_jsdata(uint16_t roll, uint16_t pitch, uint16_t yaw, uint16_t lift)
{
    //printf("save_jsdata payload: %6d %6d %6d %6d \n", payload[0], payload[1], payload[2], payload[3]);

    state.js_roll  = BOUND(-32767 ,roll - 32767 + state.offset[0], 32767); //should check for out of bounds here
    state.js_pitch = BOUND(-32767 ,pitch - 32767 + state.offset[1], 32767);
    state.js_yaw   = BOUND(-32767 ,yaw - 32767 + state.offset[2], 32767);
    state.js_lift  = BOUND(0 ,lift + state.offset[3], 65535);

    //printf("roll: %6d | pitch: %6d | yaw: %6d | lift: %6d \n", state.roll, state.pitch, state.yaw, state.lift );

}

/**
 * @brief Adds offset to the existing offset in the state offset array.
 * 
 * @param index index in offset array
 * @param offset the required offset
 */
void state_set_offset(int index, int16_t offset)
{
    state.offset[index]+= offset;
}

/**
 * @brief gets the current mode
 * 
 * @return int current mode
 * 
 * @author Koen Wösten
 */
int state_get_mode(){
    return state.next;
}