/**
 * @file check_control.c
 * @author Koen Wosten
 * @brief 
 * @version 0.1
 * @date 2022-06-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "check_check.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>


#include <inttypes.h>
#include <check.h>
#include <math.h>
#include <time.h>
#include "../../control.h"
#include "../../state/state.h"

extern struct state state;
extern uint16_t motor[4];

START_TEST(test_control_motors) {
    motors_enable();

    state.ae[0] = 200;
    state.ae[1] = 201;
    state.ae[2] = 202;
    state.ae[3] = 203;


    update_motors();

    ck_assert_uint_eq(motor[0], 200);
    ck_assert_uint_eq(motor[1], 201);
    ck_assert_uint_eq(motor[2], 202);
    ck_assert_uint_eq(motor[3], 203);
} END_TEST

START_TEST(test_control_motors_disabled) {
    motors_disable();

    state.ae[0] = 200;
    state.ae[1] = 201;
    state.ae[2] = 202;
    state.ae[3] = 203;


    update_motors();

    ck_assert_uint_eq(motor[0], 0);
    ck_assert_uint_eq(motor[1], 0);
    ck_assert_uint_eq(motor[2], 0);
    ck_assert_uint_eq(motor[3], 0);
} END_TEST

START_TEST(test_control_motors_disable_reenable) {
    motors_disable();

    state.ae[0] = 200;
    state.ae[1] = 201;
    state.ae[2] = 202;
    state.ae[3] = 203;

    update_motors();

    ck_assert_uint_eq(motor[0], 0);
    ck_assert_uint_eq(motor[1], 0);
    ck_assert_uint_eq(motor[2], 0);
    ck_assert_uint_eq(motor[3], 0);

    motors_enable();

    state.ae[0] = 200;
    state.ae[1] = 201;
    state.ae[2] = 202;
    state.ae[3] = 203;

    update_motors();

    ck_assert_uint_eq(motor[0], 200);
    ck_assert_uint_eq(motor[1], 201);
    ck_assert_uint_eq(motor[2], 202);
    ck_assert_uint_eq(motor[3], 203);

    motors_disable();
    update_motors();

    ck_assert_uint_eq(motor[0], 0);
    ck_assert_uint_eq(motor[1], 0);
    ck_assert_uint_eq(motor[2], 0);
    ck_assert_uint_eq(motor[3], 0);

    //state was wiped
    ck_assert_uint_eq(state.ae[0], 0);
    ck_assert_uint_eq(state.ae[1], 0);
    ck_assert_uint_eq(state.ae[2], 0);
    ck_assert_uint_eq(state.ae[3], 0);

} END_TEST

START_TEST(test_control_motors_clipping) {
    motors_enable();

    state.ae[0] = 700;
    state.ae[1] = 701;
    state.ae[2] = 702;
    state.ae[3] = 703;


    update_motors();

    ck_assert_uint_eq(motor[0], MAX_MOTOR_VALUE);
    ck_assert_uint_eq(motor[1], MAX_MOTOR_VALUE);
    ck_assert_uint_eq(motor[2], MAX_MOTOR_VALUE);
    ck_assert_uint_eq(motor[3], MAX_MOTOR_VALUE);
} END_TEST

START_TEST(test_control_motors_edge_cases) {
    motors_enable();
    
    state.ae[0] = 400;
    state.ae[1] = 0;
    state.ae[2] = 128; 
    state.ae[3] = 4;


    update_motors();

    ck_assert_uint_eq(motor[0], 400);
    ck_assert_uint_eq(motor[1], 0);
    ck_assert_uint_eq(motor[2], 128);
    ck_assert_uint_eq(motor[3], 4);
} END_TEST

START_TEST(test_control_motors_min_value) {
    motors_enable();
    
    motors_set_momentum(32767, 32767,32767,65535);

    update_motors();

    ck_assert_uint_ge(motor[0], MOTOR_OFFSET);
    ck_assert_uint_ge(motor[1], MOTOR_OFFSET);
    ck_assert_uint_ge(motor[2], MOTOR_OFFSET);
    ck_assert_uint_ge(motor[3], MOTOR_OFFSET);

    motors_set_momentum(32767, 32767,32767,1);

    update_motors();

    ck_assert_uint_ge(motor[0], MOTOR_OFFSET);
    ck_assert_uint_ge(motor[1], MOTOR_OFFSET);
    ck_assert_uint_ge(motor[2], MOTOR_OFFSET);
    ck_assert_uint_ge(motor[3], MOTOR_OFFSET);

    motors_set_momentum(32767,32767,32767,32767);

    update_motors();

    ck_assert_uint_ge(motor[0], MOTOR_OFFSET);
    ck_assert_uint_ge(motor[1], MOTOR_OFFSET);
    ck_assert_uint_ge(motor[2], MOTOR_OFFSET);
    ck_assert_uint_ge(motor[3], MOTOR_OFFSET);

    motors_set_momentum(-32767,32767,32767,32767);

    update_motors();

    ck_assert_uint_ge(motor[0], MOTOR_OFFSET);
    ck_assert_uint_ge(motor[1], MOTOR_OFFSET);
    ck_assert_uint_ge(motor[2], MOTOR_OFFSET);
    ck_assert_uint_ge(motor[3], MOTOR_OFFSET);

    motors_set_momentum(32767,-32767,32767,32767);

    update_motors();

    ck_assert_uint_ge(motor[0], MOTOR_OFFSET);
    ck_assert_uint_ge(motor[1], MOTOR_OFFSET);
    ck_assert_uint_ge(motor[2], MOTOR_OFFSET);
    ck_assert_uint_ge(motor[3], MOTOR_OFFSET);

    motors_set_momentum(32767,32767,-32767,32767);

    update_motors();

    ck_assert_uint_ge(motor[0], MOTOR_OFFSET);
    ck_assert_uint_ge(motor[1], MOTOR_OFFSET);
    ck_assert_uint_ge(motor[2], MOTOR_OFFSET);
    ck_assert_uint_ge(motor[3], MOTOR_OFFSET);
} END_TEST

START_TEST(test_control_motors_minimal_throttle) {
    motors_enable();
    
    motors_set_momentum(32767, 32767,32767,1);

    update_motors();

    ck_assert_uint_eq(motor[0], MOTOR_OFFSET);
    ck_assert_uint_eq(motor[1], MOTOR_OFFSET);
    ck_assert_uint_eq(motor[2], MOTOR_OFFSET);
    ck_assert_uint_eq(motor[3], MOTOR_OFFSET);

    motors_set_momentum(32767, 32767,32767,1);

    update_motors();

    ck_assert_uint_eq(motor[0], MOTOR_OFFSET);
    ck_assert_uint_eq(motor[1], MOTOR_OFFSET);
    ck_assert_uint_eq(motor[2], MOTOR_OFFSET);
    ck_assert_uint_eq(motor[3], MOTOR_OFFSET);

    motors_set_momentum(32767,32767,32767,1);

    update_motors();

    ck_assert_uint_eq(motor[0], MOTOR_OFFSET);
    ck_assert_uint_eq(motor[1], MOTOR_OFFSET);
    ck_assert_uint_eq(motor[2], MOTOR_OFFSET);
    ck_assert_uint_eq(motor[3], MOTOR_OFFSET);

    motors_set_momentum(-32767,32767,32767,1);

    update_motors();

    ck_assert_uint_eq(motor[0], MOTOR_OFFSET);
    ck_assert_uint_eq(motor[1], MOTOR_OFFSET);
    ck_assert_uint_eq(motor[2], MOTOR_OFFSET);
    ck_assert_uint_eq(motor[3], MOTOR_OFFSET);

    motors_set_momentum(32767,-32767,32767,1);

    update_motors();

    ck_assert_uint_eq(motor[0], MOTOR_OFFSET);
    ck_assert_uint_eq(motor[1], MOTOR_OFFSET);
    ck_assert_uint_eq(motor[2], MOTOR_OFFSET);
    ck_assert_uint_eq(motor[3], MOTOR_OFFSET);

    motors_set_momentum(32767,32767,-32767,1);

    update_motors();

    ck_assert_uint_eq(motor[0], MOTOR_OFFSET);
    ck_assert_uint_eq(motor[1], MOTOR_OFFSET);
    ck_assert_uint_eq(motor[2], MOTOR_OFFSET);
    ck_assert_uint_eq(motor[3], MOTOR_OFFSET);
} END_TEST


START_TEST(test_control_motors_zero_throttle) {
    motors_enable();
    
    motors_set_momentum(32767, 32767,32767,0);

    update_motors();

    ck_assert_uint_eq(motor[0], 0);
    ck_assert_uint_eq(motor[1], 0);
    ck_assert_uint_eq(motor[2], 0);
    ck_assert_uint_eq(motor[3], 0);

    motors_set_momentum(32767, 32767,32767,0);

    update_motors();

    ck_assert_uint_eq(motor[0], 0);
    ck_assert_uint_eq(motor[1], 0);
    ck_assert_uint_eq(motor[2], 0);
    ck_assert_uint_eq(motor[3], 0);

    motors_set_momentum(32767,32767,32767,0);

    update_motors();

    ck_assert_uint_eq(motor[0], 0);
    ck_assert_uint_eq(motor[1], 0);
    ck_assert_uint_eq(motor[2], 0);
    ck_assert_uint_eq(motor[3], 0);

    motors_set_momentum(-32767,32767,32767,0);

    update_motors();

    ck_assert_uint_eq(motor[0], 0);
    ck_assert_uint_eq(motor[1], 0);
    ck_assert_uint_eq(motor[2], 0);
    ck_assert_uint_eq(motor[3], 0);

    motors_set_momentum(32767,-32767,32767,0);

    update_motors();

    ck_assert_uint_eq(motor[0], 0);
    ck_assert_uint_eq(motor[1], 0);
    ck_assert_uint_eq(motor[2], 0);
    ck_assert_uint_eq(motor[3], 0);

    motors_set_momentum(32767,32767,-32767,0);

    update_motors();

    ck_assert_uint_eq(motor[0], 0);
    ck_assert_uint_eq(motor[1], 0);
    ck_assert_uint_eq(motor[2], 0);
    ck_assert_uint_eq(motor[3], 0);
} END_TEST

START_TEST(test_control_motors_constant_lift) {
    srand(time(NULL) - _i*101);

    motors_enable();
    
    uint16_t lift = rand()%65534;

    motors_set_momentum(0,0,0,lift);

    update_motors();

    //MOTOR OFFSET removed as it fuck with the quadratic equality.

    double lift1 = pow(motor[0]-MOTOR_OFFSET,2)  + pow(motor[1]-MOTOR_OFFSET,2) + pow(motor[2]-MOTOR_OFFSET,2) + pow(motor[3]-MOTOR_OFFSET,2);

    motors_set_momentum(rand()%65534-32767,rand()%65534-32767,rand()%65534-32767,lift);

    update_motors();

    double lift2 = pow(motor[0]-MOTOR_OFFSET,2)  + pow(motor[1]-MOTOR_OFFSET,2) + pow(motor[2]-MOTOR_OFFSET,2) + pow(motor[3]-MOTOR_OFFSET,2);

    //it actually overshoots a bit every time, but that is fine a bit more lift when going sideways might be a good thing.
    ck_assert_double_eq_tol(lift1, lift2, 4 * (pow(MAX_MOTOR_VALUE-MOTOR_OFFSET,2) - pow(MAX_MOTOR_VALUE-MOTOR_OFFSET-1,2)));
} END_TEST


START_TEST(test_control_fast_sqrt) {
    srand(time(NULL) - _i*101);

    uint16_t number = rand()%65534;

    ck_assert_int_eq(sqrt(number),  fast_sqrt_c(number));

} END_TEST




Suite *make_suite(void) {
	Suite *s;
	TCase *tc_core;

	s = suite_create("Control");
	tc_core = tcase_create("Control");

	//tcase_add_test(tc_core, test_packet_free);
	tcase_add_test(tc_core, test_control_motors);
    tcase_add_test(tc_core, test_control_motors_disabled);
    tcase_add_test(tc_core, test_control_motors_disable_reenable);
	tcase_add_test(tc_core, test_control_motors_clipping);
	tcase_add_test(tc_core, test_control_motors_edge_cases);
    tcase_add_test(tc_core, test_control_motors_min_value);
    tcase_add_test(tc_core, test_control_motors_zero_throttle);
    tcase_add_test(tc_core, test_control_motors_minimal_throttle);
    tcase_add_loop_test(tc_core, test_control_motors_constant_lift, 0, 5);
    tcase_add_loop_test(tc_core, test_control_fast_sqrt, 0, 5);



	suite_add_tcase(s, tc_core);

	return s;
}
