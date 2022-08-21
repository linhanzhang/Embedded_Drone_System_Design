/*------------------------------------------------------------------
 *  in4073.c -- test QR engines and sensors
 *
 *  reads ae[0-3] uart rx queue
 *  (q,w,e,r increment, a,s,d,f decrement)
 *
 *  prints timestamp, ae[0-3], sensors to uart tx queue
 *
 *  I. Protonotarios
 *  Embedded Software Lab
 *
 *  June 2016
 *------------------------------------------------------------------
 */

#include "in4073.h"
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>
#include "comms/packet.h"
#include "comms/packet_builder.h"
#include "comms/packet_tx.h"
#include "comms/packet_rx.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "app_util_platform.h"
#include "adc.h"
#include "barometer.h"
#include "gpio.h"
#include "spi_flash.h"
#include "timers.h"
#include "twi.h"
#include "hal/uart.h"
#include "control.h"
#include "mpu6050/mpu6050.h"
#include "utils/quad_ble.h"
#include "state/state.h"
#include "scheduler.h"
#include "sensors.h"
#include "fail.h"

#define RX_TIMEOUT 150000
#define BATTERY_THRESHOLD 1080 // 3-cell never below 3v and max 4.2v, lets take 3.5V as "empty" 3.5*3 = 1050

bool demo_done;

extern struct state state;

void task_state_update_f(void);
void task_toggle_led_f(void);
void task_process_packet_f(void);
void task_telemetry_f(void);
void task_check_battery_f(void);

uint8_t dequeue_wrapped(void);

/**
 * @brief Main function
 *
 * Initializes all components, defines periodic tasks and executes the scheduler.
 *
 * @author Koen Wösten
 */
int main(void)
{
	uart_init();
	gpio_init();
	timers_init();
	adc_init();
	twi_init();
	imu_init(true, 100);
	baro_init();
	sensors_init();
	spi_flash_init();
	quad_ble_init();
	// logging_init();
	state_init();

	demo_done = false;
	wireless_mode = false;

	task task_blink = {"blink", 500, 500, task_toggle_led_f, 0, 0};
	task task_state = {"state", 10, 10, task_state_update_f, 0 ,0};
	task task_process_packet = {"process", 10, 10, task_process_packet_f, 0, 0};
	task task_telemetry = {"telemetry", 15, 15, task_telemetry_f, 0, 0};
	task task_sensors = {"sensors", 10, 10, raw_timer_handler, 0, 0};
	task task_battery = {"battery", 45, 45, task_check_battery_f, 0, 0};

	// priority based on addition order
	scheduler_add_task(&task_sensors);
	scheduler_add_task(&task_state);
	scheduler_add_task(&task_process_packet);
	scheduler_add_task(&task_telemetry);
	scheduler_add_task(&task_battery);
	scheduler_add_task(&task_blink);

	// wait a bit
	nrf_delay_ms(100);

	printf("boop");

	while (!demo_done)
	{
		scheduler(); // exec periodic tasks

		//state.ae[0] = rx_queue.count;

		if (task_sensors.pending > 1 || task_state.pending > 1 || task_telemetry.pending > 1 || task_process_packet.pending > 1)
		{
			error_send("Scheduler is lagging", 21);
		}
		while (!check_timer_flag())
		{ // fill excess time with nonsense
		}
	}

	printf("\n\t Goodbye \n\n");
	nrf_delay_ms(100);

	NVIC_SystemReset();
}

/**
 * @brief task that updates the state
 *
 * @author Koen Wösten
 */
void task_state_update_f(void)
{

	static int last_state = 1;

	state_step();

	if (state.next != last_state)
	{
		nrf_gpio_pin_set(GREEN);
		nrf_gpio_pin_set(RED);
	}

	last_state = state.next;
}

/**
 * @brief task that toggle blue led
 *
 * @author Koen Wösten
 */
void task_toggle_led_f(void)
{
	nrf_gpio_pin_toggle(BLUE);
}

//[Richard Fang]
void task_telemetry_f(void)
{
	nrf_gpio_pin_toggle(YELLOW);

	struct Packet *packet = malloc(sizeof(struct Packet));

	if (packet == NULL)
	{
		return;
	}

	int err = fpacketf(packet, MSG_TELEMETRY,
					   state.next, dmp_mode,
					   FULL_P1,FULL_P2,YAW_P,
					   state.ae[0], state.ae[1], state.ae[2], state.ae[3],
					   // yaw_setpoint, state.js_pitch, state.js_yaw, state.js_lift,
					   state.phi, state.theta, state.psi,
					   state.sp, state.sq, state.sr,
					   state.sax, state.say, state.saz,
					   state.offset[2],
					   bat_volt,
					   get_time_us());

	// int err = 0;
	// printf("\rerr: %d\n", err);

	if (!err)
	{
		send_packet(packet, uart_put);
		packet_free(packet);
	}

	// #ifdef DEBUG_send_flight_data
	// struct Packet *packet = malloc(sizeof(struct Packet));
	// flight_packet_update(packet);

	//	send_flight_data(packet);
	// nrf_gpio_pin_set(YELLOW);

	//#endif
}

uint8_t dequeue_wrapped(void)
{
	return dequeue(&rx_queue);
}

//[Hanzhang Lin]
void task_process_packet_f(void)
{

	static uint32_t last_arrival = 0;
	static uint32_t delta_time = 0;
	static bool initialized = false;

	static uint8_t missed_count = 0;

	if (!initialized)
	{
		last_arrival = get_time_us();
		initialized = true;
	}

	struct Packet *packet;
	int r_err = receive_packet(&packet, dequeue_wrapped, (int *)&rx_queue.count);

	if (!r_err)
	{
		last_arrival = get_time_us(); // get last complete packet, could be problametic with 260 byte long messages. Can expect a max delay of 5 ticks = 100ms
		// act on it;

		// char str[32];
		// sprintf(str, "Got packet type %d", packet->mstype);
		// error_send(str, 32);

		// printf("%10ld %10ld \n", last_arrival, delta_time);

		switch (packet->mstype)
		{
		case CODE_MSG_CONTROL:; // empty statement to allow jump
			uint16_t roll = 0, pitch = 0, yaw = 0, lift = 0;

			fread_packetf(packet, MSG_CONTROL, &roll, &pitch, &yaw, &lift);
			state_set_jsdata(roll, pitch, yaw, lift);
			break;

		case CODE_MSG_KEY:; // empty statement to allow jump to declaration
			char key = 0;

			fread_packetf(packet, MSG_KEY, &key);
			process_key(key);

			break;

		default:
			break;
			// nothing
		}
		packet_free(packet);
	}

	delta_time = get_time_us() - last_arrival;

	if ((delta_time > RX_TIMEOUT) && (delta_time > 0) && initialized)
	{
		error_send("CONNECTION LOST", 16);
		missed_count++;
		if (state_get_mode() != MODE_PANIC && missed_count > 2) // Enter panic mode only if NOT
																// in panic mode already
		{
			state_request_mode(MODE_PANIC);
			state_request_abort();
		}
	}
	else
	{
		missed_count = missed_count ? missed_count - 1 : 0;
	}
}

/**
 * @brief task that checks the battery level
 *
 * @author Koen Wösten
 */
void task_check_battery_f(void)
{
	static uint8_t count = 0;

	if (bat_volt < BATTERY_THRESHOLD && bat_volt > 650)
	{
		count++;
		if (count > 16)
		{
			printf("Battery Empty");
			state_request_mode(MODE_PANIC);
			state_request_abort();
		}
	}
	else
	{
		if (count > 0)
		{
			count--;
		}
	}
}
