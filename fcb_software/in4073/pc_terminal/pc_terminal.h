#include <inttypes.h>


#define FIRSTBYTE(x) ((uint8_t) ((x & 0xff00) >> 8)) 
#define SECONDBYTE(x) ((uint8_t) ((x & 0x00ff))) 

void terminal_profile_begin();
void terminal_profile_end();
