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
#include "../../state/modes.h"
#include "../../state/state.h"

#define OK true
#define FAIL false

extern struct state state;
//int16_t yaw_setpoint;
//int16_t yaw_setpoint_offset;

void mock_f(struct state *state);
mode fail = {
    mock_f,
    mock_f,
    mock_f};
mode safe = {
    mock_f,
    mock_f,
    mock_f};
mode panic = {
    mock_f,
    mock_f,
    mock_f};
mode manual = {
    mock_f,
    mock_f,
    mock_f};
mode calibrate = {
    mock_f,
    mock_f,
    mock_f};
mode yaw = {
    mock_f,
    mock_f,
    mock_f};
mode full = {
    mock_f,
    mock_f,
    mock_f};
mode raw = {
    mock_f,
    mock_f,
    mock_f};

void mock_f(__attribute__ ((unused)) struct state *state){
    printf("Entered state: %s\n", __func__); // print current function
}

mode* modes[NUM_MODES] =  {&fail, &safe, &panic, &manual, &calibrate, &yaw, &full, &raw};

bool mode_transitions[NUM_MODES][NUM_MODES] = {
    {FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL},
    {OK, OK, OK, OK, OK, OK, OK, OK},
    {OK, OK, OK, FAIL, FAIL, FAIL, FAIL}, 
    {OK, OK, OK, FAIL, FAIL, FAIL, FAIL},
    {OK, OK, OK, FAIL, FAIL, FAIL, FAIL},
    {OK, OK, OK, FAIL, FAIL ,FAIL, FAIL},
    {OK, OK, OK, FAIL, FAIL ,FAIL, FAIL},
    {OK, OK, OK, FAIL, OK ,FAIL, OK}};



