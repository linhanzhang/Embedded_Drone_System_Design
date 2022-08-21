/*------------------------------------------------------------------
 *  modes.h -- definitions for different modes, and function prototypes
 *
 *  Koen Wösten
 *  
 *  11 May 2022
 *------------------------------------------------------------------
 */

#ifndef MODES_H_
#define MODES_H_

#include <stdbool.h>
#include <inttypes.h>

#define NUM_MODES 8

#define MODE_FAIL 0
#define MODE_SAFE 1
#define MODE_PANIC 2
#define MODE_MANUAL 3
#define MODE_CALIBRATE 4
#define MODE_YAW 5
#define MODE_FULL 6
#define MODE_RAW 7

struct state; //forward declaration
typedef void mode_fn(struct state *); //Typedef for a mode function

/**
 * @brief struct that contatins the behaviour of a state
 * 
 * @author Koen Wosten
 */
typedef struct {
    mode_fn *init; //initialization function for the state
    mode_fn *run;  //running function for the state
    mode_fn *exit; //exiting function for the state
} mode;

extern mode safe, panic, manual, fail, calibrate, yaw, full, raw;
extern mode *modes[NUM_MODES];
extern bool mode_transitions[NUM_MODES][NUM_MODES];

#endif //MODES_H_
