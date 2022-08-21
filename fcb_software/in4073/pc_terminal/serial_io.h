
/*------------------------------------------------------------
 * Serial I/O
 * 8 bits, 1 stopbit, no parity,
 * 115,200 baud
 *------------------------------------------------------------
 */


#ifndef SERIAL_PORT_IO_H
#define SERIAL_PORT_IO_H

#include <inttypes.h>


void serial_port_open(const char *serial_device);
void serial_port_close(void);
uint8_t serial_port_getchar(void);
int serial_port_putchar(char c);
int serial_port_getchar_int(uint8_t *c);
int serial_port_bytes_available(void);

#endif // SERIAL_PORT_IO_H
