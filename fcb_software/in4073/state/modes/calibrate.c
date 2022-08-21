/*------------------------------------------------------------------
 *  calibrate.c -- function for calibrate sensor data in DMP/RAW mode
 *
 *  siyuan fang
 *
 *  7 June 2022
 *------------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "../state.h"
#include "sensors.h"

#include "../modes.h"
#include "../../control.h"
#include "../fail.h"

// This is the declaration of variables and functions regarding calibration mode
#define CALIBRATION_TIMES 256

int32_t calibration_counter = 0;

int32_t sp_offset;
int32_t sq_offset;
int32_t sr_offset;
int32_t sax_offset;
int32_t say_offset;
int32_t saz_offset;

int32_t phi_offset;
int32_t theta_offset;
int32_t psi_offset;

void calibration_Update();
void calibration_Printoffset();
void calibration_save_to_state(struct state *state);
void calibration_init(struct state *state);
void calibrate_run(struct state *state);

mode calibrate = {
    calibration_init,
    calibrate_run,
    calibration_save_to_state};

/*------------------------------------------------------------------
 *  calibrate_run function
 *
 *  Tianrui Mao
 *
 *  7 May 2022
 *------------------------------------------------------------------
 */

void calibrate_run(struct state *state)
{
    motors_disable();
    /* Switch back to safe when done */
    if (calibration_counter < CALIBRATION_TIMES)
    {
        calibration_Update(state);
        calibration_counter++;
    }
    else
    {
        state_request_mode(MODE_SAFE);
    }
}

void calibration_init(struct state *state)
{
    motors_disable();

    calibration_counter = 0;
    phi_offset = 0;
    theta_offset = 0;
    psi_offset = 0;
    sp_offset = 0;
    sq_offset = 0;
    sr_offset = 0;
    sax_offset = 0;
    say_offset = 0;
    saz_offset = 0;

    state->phi_offset = 0;
    state->theta_offset = 0;
    state->psi_offset = 0;
    state->sax_offset = 0;
    state->say_offset = 0;
    state->saz_offset = 0;
    state->sp_offset = 0;
    state->sq_offset = 0;
    state->sr_offset = 0;
}

//[Koen Wosten]
void calibration_Update(struct state *state)
{
    phi_offset += state->phi;
    theta_offset += state->theta;
    psi_offset += state->psi;
    sp_offset += state->sp;
    sq_offset += state->sq;
    sr_offset += state->sr;
    sax_offset += state->sax;
    say_offset += state->say;
    saz_offset += state->saz;
}

void calibration_save_to_state(struct state *state)
{
    state->phi_offset = phi_offset / calibration_counter;
    state->theta_offset = theta_offset / calibration_counter;
    state->psi_offset = psi_offset / calibration_counter;
    state->sp_offset = sp_offset / calibration_counter;
    state->sq_offset = sq_offset / calibration_counter;
    state->sr_offset = sr_offset / calibration_counter;
    state->sax_offset = sax_offset / calibration_counter;
    state->say_offset = say_offset / calibration_counter;
    state->saz_offset = saz_offset / calibration_counter;
  
    calibration_counter = 0;

}

void calibration_Printoffset()
{
    if (calibration_counter > 1000)
    {
        printf("%ld %ld %ld %ld %ld %ld %ld %ld %ld\n", sp_offset, sq_offset, sr_offset, sax_offset, say_offset, saz_offset, phi_offset, theta_offset, phi_offset);
    }
}