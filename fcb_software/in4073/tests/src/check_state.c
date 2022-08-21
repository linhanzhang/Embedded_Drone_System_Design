/**
 * @file check_state.c
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

#include <inttypes.h>
#include <check.h>
#include "../../state/modes.h"
#include "../../state/state.h"

extern struct state state;

START_TEST(test_state_start_safe) {
    state_init();

    ck_assert_int_eq(state.next, MODE_SAFE);
} END_TEST

Suite *make_suite(void) {
	Suite *s;
	TCase *tc_core;

	s = suite_create("State");
	tc_core = tcase_create("State");

	tcase_add_test(tc_core, test_state_start_safe);

	suite_add_tcase(s, tc_core);

	return s;
}
