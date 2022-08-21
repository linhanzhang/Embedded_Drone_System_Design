/**
 * @file mock_fail.c
 * @author Koen Wosten
 * @brief 
 * @version 0.1
 * @date 2022-06-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>

void error_send(char* msg, __attribute__ ((unused)) uint8_t n){
	printf("Error: %s", msg);
}