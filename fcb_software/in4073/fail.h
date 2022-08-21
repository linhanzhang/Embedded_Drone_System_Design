#ifndef _FAIL_H
#define _FAIL_H

#include <inttypes.h>

/**
 * @brief Function that sends an error message to the PC
 * 
 * @author Koen Wosten
 * 
 * @param msg the message
 * @param n number of bytes of the message
*/
void error_send(char* msg, uint8_t n);

#endif // _FAIL_H