/*------------------------------------------------------------
 * Simple pc terminal in C
 *
 * Arjan J.C. van Gemund (+ mods by Ioannis Protonotarios)
 *
 * read more: http://mirror.datenwolf.net/serial/
 *------------------------------------------------------------
 */

#include "pc_terminal.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <termios.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>
#include "serial_io.h"
#include "console_io.h"
#include "joystick.h"
#include "flight-packet-rx.h"
#include "../comms/packet.h"
#include "../comms/packet_builder.h"
#include "../comms/packet_tx.h"
#include "../comms/packet_rx.h"
#include "../control_packet_rx.h"


uint16_t* create_joystick_payload(JoystickData *jsdata);

#define DEBUG

struct js_event js;

int 		fd;

/*	
 * Author: siyuan fang
 * 		-- payload for socket communication
 */

#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <pthread.h>

pthread_mutex_t lock;
#pragma pack(1)
int nread;
int sock;
typedef struct payload_t {
	uint16_t mode;
	int16_t ae_0;
	int16_t ae_1;
	int16_t ae_2;
	int16_t ae_3;
    int16_t phi;
    int16_t theta;
	int16_t psi;
    int16_t sp;
    int16_t sq;
	int16_t sr;
    int16_t sax;
    int16_t say;
	int16_t saz;
	uint16_t P1;
	uint16_t P2;
	uint16_t P_YAW;
	int16_t yaw_setpoint;
	int16_t bat_volt;

} payload_gui;

void delay(int seconds)
{
    int milliseconds = 1000 * seconds;
    clock_t start_time = clock();
    while (clock() < start_time + milliseconds)
        ;
}
/*	
 * Author: siyuan fang
 * 		-- socket communication sendMsg function
 */
void sendMsg(int sock, void* msg, uint32_t msgsize)
{
    if (write(sock, msg, msgsize) < 0)
    {
        close(sock);
        exit(1);
    }
    // printf("Message sent (%d bytes).\n", msgsize);
    return;
}

/*	
 * Author: siyuan fang
 * 		-- socket communication thread
 */

void* thread_socket(){

	const int PORT = 2300;
	const char* SERVERNAME = "localhost";
    int BUFF_SIZE = sizeof(payload_gui);
    char buff[BUFF_SIZE];
	
	
	time_t t;
	srand((unsigned) time(&t));

	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	inet_pton(AF_INET, SERVERNAME, &server_address.sin_addr);
	server_address.sin_port = htons(PORT);
	
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		term_puts("ERROR: Socket creation failed\n");
		return NULL;
	}

	if (connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
		term_puts("ERROR: Unable to connect to server\n");
		return NULL;
	}
	
	while(1){
		pthread_mutex_lock(&lock);
		payload_gui data;
		data.mode = mode_rx;
		data.ae_0 = ae_rx[0];
		data.ae_1= ae_rx[1];
		data.ae_2 = ae_rx[2];
		data.ae_3 = ae_rx[3];
		data.phi = phi_rx;
		data.theta = theta_rx;
		data.psi = psi_rx;
		data.sp = sp_rx;
		data.sq = sq_rx;
		data.sr = sr_rx;
		data.sax = sax_rx;
		data.say = say_rx;
		data.saz = saz_rx;
		data.P1 = FULL_P1_RX;
		data.P2 = FULL_P2_RX;
		data.P_YAW = YAW_K_RX;
		data.yaw_setpoint = yaw_setpoint_rx;
		data.bat_volt = bat_volt_rx;

		pthread_mutex_unlock(&lock);
		// fprintf(stderr, "\nSending id=%6d, counter=%6d, temp=%6d\n", data.sp, data.sq, data.sr);
		// delay(10);
		sendMsg(sock, &data, BUFF_SIZE);
		bzero(buff, BUFF_SIZE);
		nread = read(sock, buff, BUFF_SIZE);

		
	}
	return NULL;
}


/*	
 * 		-- main thread
 */
char d;			// parameter for keyboard input
uint8_t c;
int done_this_loop;

long trigger = 15; /* 15ms */
struct timespec before, now;
 
JoystickData *jsdata;
struct js_event js;


void* thread_main_loop()
{

	while(1)
	{
		long msec = 0;
		long nsec = 0;

		done_this_loop = false;

		clock_gettime(CLOCK_MONOTONIC, &before);

		//printf("test2\n");

		
		// struct pollfd pfds[1];

		// pfds[0].fd = fd_serial_port;
		// pfds[0].eve		// struct pollfd pfds[1];

		// pfds[0].fd = fd_serial_port;
		// pfds[0].events = POLLIN;nts = POLLIN;
			

		do { //keep doing the stuff in this do, untill trigger is reached
			Joystick_getValue(&fd, &js, jsdata);
			//printf("test3\n");

			// if (serial_port_getchar_int(&c) != -1)
			// {
			struct Packet *r_packet;
			//printf("%c", c);

			if (serial_port_bytes_available()){
				//TODO: currently not great
				int cond = 1;
				int r_err = receive_packet(&r_packet, serial_port_getchar, &cond);
				if(r_err == 0){
					int err;
					//printf("packetlen: %d, type %d\n", r_packet->len, r_packet->mstype);

					switch (r_packet->mstype)
					{
					case CODE_MSG_TELEMETRY:
						{
						//TODO: check if these are all the right sizes
						err = fread_packetf(r_packet, MSG_TELEMETRY, &mode_rx, &dmp_rx,
											&FULL_P1_RX, &FULL_P2_RX, &YAW_K_RX,
											&ae_rx[0], &ae_rx[1], &ae_rx[2], &ae_rx[3],
											&phi_rx, &theta_rx, &psi_rx,
											&sp_rx, &sq_rx, &sr_rx,
											&sax_rx, &say_rx, &saz_rx,
											&yaw_setpoint_rx, &bat_volt_rx, &time_stamp_rx);
						
						if(!err){
							log_file_write();
						}

						break;
						}

					case CODE_MSG_ERROR:
						{
						char *msg = malloc(r_packet->len);
						err = fread_packetf(r_packet, MSG_ERROR(r_packet->len), msg);
						if(!err){
							printf("\e[38;5;196mERROR: %s\n\e[0m", msg);
						}
						free(msg);	
						}			

					default:
						break;
					};
					packet_free(r_packet);

				
				}
			} 

			if ((d = term_getchar_nb()) != -1) {
				if (d == 27){
                    if ((d = term_getchar_nb()) != -1){
                        if (d == 91){
                            if ((d = term_getchar_nb()) != -1){

                            }
                            else {
                                struct Packet *packet = malloc(sizeof(struct Packet));
								fpacketf(packet, MSG_KEY, 27);
								send_packet(packet, serial_port_putchar);
								packet_free(packet);
                            }
                        }
                    } else {
                        	struct Packet *packet = malloc(sizeof(struct Packet));
							fpacketf(packet, MSG_KEY, 27);
							send_packet(packet, serial_port_putchar);
							packet_free(packet);
                    }
                }
				char str[64];
				sprintf(str, "key prcssed: %c\n", d);
				term_puts(str);

			
				struct Packet *packet = malloc(sizeof(struct Packet));
				fpacketf(packet, MSG_KEY, d);
				send_packet(packet, serial_port_putchar);
				packet_free(packet);
			}


			//printf("6\n");

			// //check for panic button
			if(jsdata->button[0] && !done_this_loop)
			{
				printf("activated\n");
				struct Packet *packet = malloc(sizeof(struct Packet));
				fpacketf(packet, MSG_KEY, '1');
				send_packet(packet, serial_port_putchar);
				packet_free(packet);
				//send_key_packet('1'); //mode 1 - PANIC
				done_this_loop = true;
			}

			//printf("7\n");

    		clock_gettime(CLOCK_MONOTONIC, &now);
			nsec = (now.tv_nsec - before.tv_nsec) / 1.0e6;
			msec = round(nsec); // Convert nanoseconds to milliseconds
			msec += (now.tv_sec - before.tv_sec) * 1000;

		} while ( msec < trigger );

		printf("%ld msec\n", msec);
		struct Packet *packet = malloc(sizeof *packet);
		uint16_t* payload = create_joystick_payload(jsdata);
		fpacketf(packet, MSG_CONTROL, payload[0], payload[1], payload[2], payload[3]);
		send_packet(packet, serial_port_putchar);
		packet_free(packet);
		free(payload);
	}

	JoystickData_release(jsdata);
	// // sleep(2);
	// while(1){
	// 	long msec = 0;
	// 	long nsec = 0;
	// 	// done_this_loop = false;

	// 	clock_gettime(CLOCK_REALTIME, &before);
		
	// 	// struct pollfd pfds[1];

	// 	// pfds[0].fd = fd_serial_port;
	// 	// pfds[0].events = POLLIN;
			

	// 	do { //keep doing the stuff in this do, untill trigger is reached
	// 		// Joystick_getValue(&fd, &js, jsdata);

	// 		// poll(pfds, 1, 1);
	// 		// if (pfds[0].revents & POLLIN)

	// 			if (serial_port_getchar_int(&c) != -1)
	// 			{
	// 				#ifdef DEBUG
	// 				//term_putchar(c);
	// 				#endif
	// 				pthread_mutex_lock(&lock);

	// 				flight_packet_parse(c);

	// 				pthread_mutex_unlock(&lock);
	// 			}
			

	// 		//any key pressed
	// 		if ((d = term_getchar_nb()) != -1) {
	// 			if (d == 27){
	// 				if ((d = term_getchar_nb()) != -1){
	// 					if (d == 91){
	// 						if ((d = term_getchar_nb()) != -1){

	// 						}
	// 						else {
	// 							send_key_packet(27);
	// 						}
	// 					}
	// 				} else {
	// 					send_key_packet(27);
	// 				}
	// 			}
	// 			send_key_packet(d);
	// 		}

	// 		//check for panic button
	// 		// if(jsdata->button[0] && !done_this_loop)
	// 		// {
	// 		// 	send_key_packet('1'); //mode 1 - PANIC
	// 		// 	done_this_loop = true;
	// 		// }


	// 		clock_gettime(CLOCK_REALTIME, &now);
	// 		nsec = (now.tv_nsec - before.tv_nsec) / 1.0e6;
	// 		msec = round(nsec); // Convert nanoseconds to milliseconds
	// 		msec += (now.tv_sec - before.tv_sec) * 1000;

	// 	} while ( msec < trigger );

	// 	// uint8_t* payload = create_joystick_payload(jsdata);
	// 	uint8_t* payload = (uint8_t*) malloc(8);
	// 	int axis = 0;
	// 	for (int i = 0; i< 4; i++){
	// 		axis = 32768;
			
	// 		if(i == 3){
	// 			axis = 0;
	// 		}

	// 		payload[2*i] = FIRSTBYTE(axis);
	// 		payload[2*i+1]  = SECONDBYTE(axis);
	// 	}
	// 	send_control_packet(payload);
	// }
	return NULL;
}


/*----------------------------------------------------------------
 * main -- execute terminal
 *----------------------------------------------------------------
 */
int main(int argc, char **argv)
{
	//char d;			// parameter for keyboard input
	//uint8_t c;

	term_initio();
	term_puts("\nTerminal program - Embedded Real-Time Systems\n");

	// if no argument is given at execution time, /dev/ttyUSB0 is assumed
	// asserts are in the function
	if (argc == 1 || argc == 2) {
		serial_port_open("/dev/ttyUSB0");
	} else if (argc == 3) {
		serial_port_open(argv[2]);
	} else {
		printf("wrong number of arguments\n");
		return -1;
	}

	char path_to_joystick[64];
	if (argc == 1) {
		strcpy(path_to_joystick, "/dev/input/js0");
	} else if (argc == 2 || argc == 3) {
		strcpy(path_to_joystick, argv[1]);
	} else {
		fprintf(stderr, "too many arguments. 0 argument = default path (js0), 1 argument = path to joystick");
		return 1;
	}
	fprintf(stderr, "using %s\n", path_to_joystick);


	JoystickData_init(&fd, path_to_joystick, &js);
	jsdata = JoystickData_create();

	log_file_init();

	term_puts("Type ^C to exit\n");

    pthread_t thread_1;
    pthread_t thread_2;
	

	if (pthread_mutex_init(&lock, NULL) != 0) {
        term_puts("Mutex init has failed\n");
        return 1;
    }

    pthread_create(&thread_1, NULL, thread_main_loop, NULL);
    pthread_create(&thread_2, NULL, thread_socket, NULL);
    pthread_join(thread_1, NULL);
    pthread_join(thread_2, NULL); 

	pthread_mutex_destroy(&lock);

	// close(sock);

	term_exitio();
	serial_port_close();
	term_puts("\n<exit>\n");

	return 0;
}

uint16_t* create_joystick_payload(JoystickData *jsdata){ 
		Joystick_getValue(&fd, &js, jsdata);

		uint16_t* payload = malloc(8);
		uint16_t axis = 0;
		
		for (int i = 0; i< 4; i++){
			axis = jsdata->axis[i] + 32767;
			
			if(i == 3){
				axis = 65534 - axis;
			}

			payload[i] = axis;

		}
		return payload;
}

// siyuan fang: profiling in pc_terminal side

struct timespec profile_before, profile_now;
long profile_msec = 0;
long profile_nsec = 0;

void terminal_profile_begin() {
	clock_gettime(CLOCK_MONOTONIC, &profile_before);
}


void terminal_profile_end() {
	clock_gettime(CLOCK_MONOTONIC, &profile_now);
	profile_nsec = (profile_now.tv_nsec - profile_before.tv_nsec) / 1.0e6;
	profile_msec = round(profile_nsec); // Convert nanoseconds to milliseconds
	profile_msec += (profile_now.tv_sec - profile_before.tv_sec) * 1000;
	//fprintf(stderr, "profile result = %ld\n", profile_msec);
}