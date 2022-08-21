/**
 * @file check_protocol.c
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

#include <inttypes.h>
#include <check.h>
#include "../../comms/packet.h"


START_TEST(test_packet_serialize) {
	struct Packet *packet = malloc(sizeof(struct Packet));
	// struct Packet *packet = malloc(sizeof(struct Packet));
	packet->len = 8;
	packet->seq = 1;
	packet->mstype = 3;

	char payload[8] = "payload";
	char returned_payload[8];

	packet->payload = (uint8_t *)malloc(packet->len);
	memcpy(packet->payload, payload, packet->len);

	uint8_t *packet_serialized = packet_serialize(packet);

	memcpy(returned_payload, packet->payload, packet->len);

	ck_assert_uint_eq(MAGIC_HEADER, packet_serialized[MAGIC]);
	ck_assert_uint_eq(packet->len, packet_serialized[LEN]);
	ck_assert_uint_eq(packet->seq, packet_serialized[SEQ]);
	ck_assert_uint_eq(packet->mstype, packet_serialized[MSTYPE]);
	ck_assert_mem_eq(packet->payload, returned_payload, packet->len);

	packet_free(packet);
	free(packet_serialized);

} END_TEST


START_TEST(test_packet_deserialize) {
	uint8_t packet_serialized[8] = {0xFF, 0x03, 0x08, 0x05, 0x10, 0x02, 0x03, 0x29};

	// | #Bytes 	| 1     	| 1   	| 1   	| 1    | 0-255   	| 1   	|
    // |--------	|-------	|-----	|-----	|----- |---------	|-----	|
    // | Name   	| MAGIC 	| LEN 	| SEQ 	| TYPE | PAYLOAD 	| CRC 	|

	struct Packet *packet = packet_deserialize(packet_serialized);
	
	ck_assert_ptr_nonnull(packet);

	ck_assert_uint_eq(packet->len, packet_serialized[LEN]);
	ck_assert_uint_eq(packet->seq, packet_serialized[SEQ]);
	ck_assert_uint_eq(packet->mstype, packet_serialized[MSTYPE]);

	uint8_t returned_payload[packet_serialized[LEN]];
	memcpy(returned_payload, packet->payload, packet_serialized[LEN]);

	ck_assert_mem_eq(packet->payload, returned_payload, packet->len);

	packet_free(packet);

} END_TEST

START_TEST(test_packet_deserialize_wrong_crc) {
	uint8_t packet_serialized[8] = {0xFF, 0x03, 0x08, 0x05, 0x10, 0x02, 0x03, 0x21};

	// | #Bytes 	| 1     	| 1   	| 1   	| 1    | 0-255   	| 1   	|
    // |--------	|-------	|-----	|-----	|----- |---------	|-----	|
    // | Name   	| MAGIC 	| LEN 	| SEQ 	| TYPE | PAYLOAD 	| CRC 	|

	struct Packet *packet = packet_deserialize(packet_serialized);

	ck_assert_ptr_null(packet);

} END_TEST

START_TEST(test_packet_deserialize_wrong_magic) {
	uint8_t packet_serialized[8] = {0x22, 0x03, 0x08, 0x05, 0x10, 0x02, 0x03, 0x29};

	// | #Bytes 	| 1     	| 1   	| 1   	| 1    | 0-255   	| 1   	|
    // |--------	|-------	|-----	|-----	|----- |---------	|-----	|
    // | Name   	| MAGIC 	| LEN 	| SEQ 	| TYPE | PAYLOAD 	| CRC 	|

	struct Packet *packet = packet_deserialize(packet_serialized);

	ck_assert_ptr_null(packet);

} END_TEST

Suite *make_suite(void) {
	Suite *s;
	TCase *tc_core;

	s = suite_create("Protocol");
	tc_core = tcase_create("Protocol");

	//tcase_add_test(tc_core, test_packet_free);
	tcase_add_test(tc_core, test_packet_serialize);
	tcase_add_test(tc_core, test_packet_deserialize);
	tcase_add_test(tc_core, test_packet_deserialize_wrong_crc);
	tcase_add_test(tc_core, test_packet_deserialize_wrong_magic);


	suite_add_tcase(s, tc_core);

	return s;
}
