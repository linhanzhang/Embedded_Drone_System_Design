/*------------------------------------------------------------------
 *  state.h -- state struct, function prototpes
 *
 *  Koen Wösten
 *  
 *  11 May 2022
 *------------------------------------------------------------------
 */

#ifndef STATE_H_
#define STATE_H_

#include <inttypes.h>
#include "modes.h"



struct state
{
    int next;
    int requested;
    int requested_abort; 
    int calibrated; // DMP: use the default filter(in other mode)  RAW:use self-defined filters(when in raw mode)

    int16_t offset[4];
    int16_t js_yaw;
    int16_t js_pitch;
    int16_t js_roll;
    uint16_t js_lift; //Only positive lift really makes sense, don't think we have rotors that can spin backwards

    uint16_t ae[4];

    int16_t phi, theta, psi;
	int16_t sp, sq, sr;
    int16_t sax, say, saz;
	uint16_t bat_volt, temperature, pressure;

    int16_t sp_offset; //Fixed: offsets can cause overflows etc, should properly bound stuff -> indeed we have overflows, so richard changed it to int32_t
    int16_t sq_offset;
    int16_t sr_offset;
    int16_t sax_offset;
    int16_t say_offset;
    int16_t saz_offset;

    int16_t phi_offset;
    int16_t theta_offset;
    int16_t psi_offset;

  

};

extern uint8_t YAW_P;
extern uint8_t FULL_P1;
extern uint8_t FULL_P2;
extern uint8_t HEIGHT_P;

// for debug
extern int32_t buttered_sp;
extern int32_t buttered_sq;


extern int32_t sp_filtered;
extern int32_t sq_filtered;


extern int64_t sp_filter_offset;
extern int64_t sq_filter_offset;

void state_init(void);
void state_step(void);
void state_request_mode(int requested);
void state_request_abort();
void state_next_mode();
int state_get_mode(void);
void state_set_jsdata(uint16_t roll, uint16_t pitch, uint16_t yaw, uint16_t lift);
void state_set_offset(int index, int16_t offset);

#endif //STATE_H_