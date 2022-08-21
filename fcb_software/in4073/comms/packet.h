#ifndef PACKET_H_
#define PACKET_H_

#include <inttypes.h>

/**
 * @brief Packet struct.
 * 
 * Contains all information to craft a packet. 
 * magic header, length of payload, sequence number. message type and payload pointer
 */
struct Packet {
    uint8_t magic;    //Starting header
    uint8_t len;      //Length in bytes of payload
    uint8_t seq;      //Packet sequence number
    uint8_t mstype;   //Message type
    uint8_t* payload; //Pointer to payload
};

#define FIRSTBYTE(x) ((uint8_t) ((x & 0xff00) >> 8)) 
#define SECONDBYTE(x) ((uint8_t) ((x & 0x00ff))) 

//Define Message types
typedef struct m_format {uint8_t x[6]; } m_format; //TYPE, SIZE, #UINT8, #UINT16, $UINT32, STR_ENDING (true/false)

#define MSG_CONTROL     ((m_format) {{CODE_MSG_CONTROL, 8  , 0 , 4, 0, 0}}) //Message for joystick updates
#define MSG_TELEMETRY   ((m_format) {{CODE_MSG_TELEMETRY, 39 , 5, 15, 1,0}}) //Message for telemetry back to pc
#define MSG_KEY         ((m_format) {{CODE_MSG_KEY, 1 , 1, 0, 0, 0}}) //Message for keystoke updates
#define MSG_ERROR(n)    ((m_format) {{CODE_MSG_ERROR, (n), 0 , 0, 0 , 1}}) //Message for erros back to the PC


/* CODES FOR MESSAGE TYPES */
#define CODE_MSG_CONTROL 1
#define CODE_MSG_TELEMETRY 2
#define CODE_MSG_KEY 3
#define CODE_MSG_ERROR 4


#define MAX_PACKET_SIZE 260
#define MAX_PAYLOAD_SIZE 255

//Define locations
#define MAGIC 0
#define LEN 1
#define SEQ 2
#define MSTYPE 3
#define PAYLOAD 4 

#define MAGIC_HEADER 0xFF

uint8_t* packet_serialize(struct Packet *packet);
struct Packet *packet_deserialize(uint8_t *data);
void packet_free(struct Packet *packet) ;
int packet_format_is_valid(m_format format);

#endif /* PACKET_H_ */
