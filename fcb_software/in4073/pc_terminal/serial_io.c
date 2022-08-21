
/*------------------------------------------------------------
 * Serial I/O
 * 8 bits, 1 stopbit, no parity,
 * 115,200 baud
 *------------------------------------------------------------
 */

#include <inttypes.h>
#include <termios.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <sys/ioctl.h>
//#include "joystick.h"

#include "serial_io.h"


static int fd_serial_port;

//struct js_event js;
/*
 * Open the terminal I/O interface to the serial/pseudo serial port.
 *
 */
void serial_port_open(const char *serial_device)
{
	char *name;
	int result;
	struct termios tty;

	fd_serial_port = open(serial_device, O_RDWR | O_NOCTTY);

	assert(fd_serial_port>=0);

	result = isatty(fd_serial_port);
	assert(result == 1);

	name = ttyname(fd_serial_port);
	assert(name != 0);

	result = tcgetattr(fd_serial_port, &tty);
	assert(result == 0);

	tty.c_iflag = IGNBRK; /* ignore break condition */
	tty.c_oflag = 0;
	tty.c_lflag = 0;

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; /* 8 bits-per-character */
	tty.c_cflag |= CLOCAL | CREAD; /* Ignore model status + read input */

	cfsetospeed(&tty, B115200);
	cfsetispeed(&tty, B115200);

	tty.c_cc[VMIN]  = 0;
	tty.c_cc[VTIME] = 1; // TODO check cpu usage

	tty.c_iflag &= ~(IXON|IXOFF|IXANY);

	result = tcsetattr (fd_serial_port, TCSANOW, &tty); /* non-canonical */

	tcflush(fd_serial_port, TCIOFLUSH); /* flush I/O buffer */
}


void serial_port_close(void)
{
	int 	result;

	result = close(fd_serial_port);
	assert (result==0);
}


uint8_t serial_port_getchar()
{
	int8_t result;
	uint8_t c;

	do {
		result = read(fd_serial_port, &c, 1);
	} while (result != 1);

	//	assert(result == 1);
	return c;
}



int serial_port_putchar(char c)
{
	int result;

	do {
		result = (int) write(fd_serial_port, &c, 1);
	} while (result == 0);

	assert(result == 1);
	return result;
}

int serial_port_getchar_int(uint8_t *c)
{
	int result;

	result = read(fd_serial_port, c, 1);

	if (result == 0)
		return -1;

	else
	{
		assert(result == 1);
		
		return 1;
	}
}

/**
 * @brief returns amaount of available bytes on the serial port
 * 
 * @return int number of bytes
 * 
 * @author Koen WOsten
 */
int serial_port_bytes_available(){
	int bytes;
	ioctl(fd_serial_port, FIONREAD, &bytes);

	return bytes;
}
