/**
 * @file check_check.c
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
#include <time.h>

int main(void) {

	//doing it in test cases now, as each test case is apparently a new fork
	//srand(time(NULL));

	int no_failed = 0;
	
	SRunner *sr;
	
	sr = srunner_create (make_suite ());
	//srunner_add_suite (sr, make_control_suite ());
	
	srunner_run_all(sr, CK_VERBOSE);
	no_failed = srunner_ntests_failed(sr);
	srunner_free(sr);	

	return ((no_failed == 0) ? CK_PASS : CK_FAILURE) - 1;
}
