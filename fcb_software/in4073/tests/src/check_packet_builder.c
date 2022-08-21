/**
 * @file check_packet_builder.c
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
#include <stdio.h>


#include <inttypes.h>
#include <check.h>
#include "../../comms/packet_builder.h"


START_TEST(test_packet_builder) {
	struct Packet *packet = malloc(sizeof(struct Packet));
	fpacketf(packet, MSG_CONTROL, 1, 2, 3, 4);

	ck_assert_uint_eq(packet->len, MSG_CONTROL.x[1]);
	ck_assert_uint_eq(packet->mstype, MSG_CONTROL.x[0]);

	
	for(int i = 0; i < MSG_CONTROL.x[3] * 2 + MSG_CONTROL.x[2]; i+=2){
		ck_assert_uint_eq((((uint16_t) packet->payload[i] ) << 8) + packet->payload[i+1], i/2 + 1);
	}

} END_TEST

START_TEST(test_packet_build_msg_telemetry) {

	struct Packet *packet = malloc(sizeof(struct Packet));
	fpacketf(packet, MSG_TELEMETRY,
							1, 2,
							3, 4, 5,
							6, 7, 8, 9,
							10, 11, 12,
							13, 14, 15,
							16, 17, 18,
							19, 20);

	ck_assert_uint_eq(packet->len, MSG_TELEMETRY.x[1]);
	ck_assert_uint_eq(packet->mstype, MSG_TELEMETRY.x[0]);

	int i;
	for(i = 0; i < MSG_TELEMETRY.x[2]; i++){
		ck_assert_uint_eq(packet->payload[i], i + 1);
	}
	for(int j = i; j < MSG_TELEMETRY.x[3] * 2 + MSG_TELEMETRY.x[2]; j+=2, i++){
		ck_assert_uint_eq((((uint16_t) packet->payload[j] ) << 8) + packet->payload[j+1], i + 1);
	}


} END_TEST

START_TEST(test_packet_build_msg) {
	struct Packet *packet = malloc(sizeof(struct Packet));

	unsigned char payload[8] = "payload";

	fpacketf(packet, MSG_ERROR(8), payload);

	ck_assert_uint_eq(packet->len, MSG_ERROR(8).x[1]);
	ck_assert_uint_eq(packet->mstype, MSG_ERROR(8).x[0]);
	ck_assert_mem_eq(packet->payload, payload, 8);

} END_TEST

START_TEST(test_packet_read_msg) {
	struct Packet *packet = malloc(sizeof(struct Packet));

	packet->len = 8;
	packet->seq = 1;
	packet->mstype = 4;

	char payload[8] = "payload";

	packet->payload = (uint8_t *)malloc(packet->len);
	memcpy(packet->payload, payload, packet->len);


	char str[8];
	fread_packetf(packet, MSG_ERROR(8), str);

	ck_assert_str_eq(payload,str);
	ck_assert_uint_eq(packet->len, MSG_ERROR(8).x[1]);
	ck_assert_uint_eq(packet->mstype, MSG_ERROR(8).x[0]);
} END_TEST


START_TEST(test_packet_read_msg_telemetry) {
	uint8_t mode_rx, dmp, P1, P2, P3;
	int16_t values[MSG_TELEMETRY.x[3]];
	uint32_t time;

	struct Packet *packet = malloc(sizeof(struct Packet));

	packet->len = MSG_TELEMETRY.x[1];
	packet->seq = 1;
	packet->mstype = MSG_TELEMETRY.x[0];

	uint8_t payload[39] = {1,2,3,4,5,0,6,0,7,0,8,0,9,0,10,0,11,0,12,0,13,0,14,0,15,0,16,0,17,0,18,0,19,0,20,0,0,0,21};

	packet->payload = payload;

	fread_packetf(packet, MSG_TELEMETRY, &mode_rx, &dmp, &P1, &P2, &P3,
										&values[0], &values[1], &values[2], &values[3],  
										&values[4], &values[5], &values[6],
										&values[7], &values[8], &values[9],
										&values[10], &values[11], &values[12],
										&values[13], &values[14], &time);

	ck_assert_uint_eq(packet->len, MSG_TELEMETRY.x[1]);
	ck_assert_uint_eq(packet->mstype, MSG_TELEMETRY.x[0]);

	ck_assert_uint_eq(mode_rx, 1);
	ck_assert_uint_eq(dmp, 2);
	ck_assert_uint_eq(P1, 3);
	ck_assert_uint_eq(P2, 4);
	ck_assert_uint_eq(P3, 5);
	ck_assert_uint_eq(time, 21);

	
	for(int i = 0; i < MSG_TELEMETRY.x[3]; i++){
		ck_assert_uint_eq(values[i], i+6);
	}


} END_TEST







Suite *make_suite(void) {
	Suite *s;
	TCase *tc_core;

	s = suite_create("Packet Builder");
	tc_core = tcase_create("Packet Builder");

	//tcase_add_test(tc_core, test_packet_free);
	tcase_add_test(tc_core, test_packet_builder);
	tcase_add_test(tc_core, test_packet_build_msg);
	tcase_add_test(tc_core, test_packet_build_msg_telemetry);
	tcase_add_test(tc_core, test_packet_read_msg);
	tcase_add_test(tc_core, test_packet_read_msg_telemetry);


	suite_add_tcase(s, tc_core);

	return s;
}
