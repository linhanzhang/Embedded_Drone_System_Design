/**
 * @file mock_barometer.c
 * @author Koen Wosten
 * @brief 
 * @version 0.1
 * @date 2022-06-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdlib.h>

int32_t pressure;
int32_t temperature;

void read_baro(void){
    pressure += rand()%512 - 256; // not a clue what these values are supposed to be
    temperature += rand()%4 - 2;
}
void baro_init(void){
    pressure = 400; // not a clue what these values are supposed to be
    temperature = 23;
}

