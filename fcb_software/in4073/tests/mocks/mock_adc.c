/**
 * @file mock_adc.c
 * @author Koen Wosten
 * @brief 
 * @version 0.1
 * @date 2022-06-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdlib.h>
#include <inttypes.h>
uint16_t bat_volt;

void adc_init(void){
    bat_volt = 1080;
    return;
}

void adc_request_sample(void){
    bat_volt += (rand()%20) - 10;
    return;
}