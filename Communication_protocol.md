# Communication Protocol  
### Group 16 
### IN4140 - Embedded Systems Lab


#### Packet Format

This section shows the serialized message format of Communication Protocol Group 16 (CP16) packets (the format is inspired by the [MAVlink](https://mavlink.io/en/guide/serialization.html) ) packet format.


Below is the over-the-wire format for a CP16 packet (the in-memory representation might differ).

| #Bytes 	| 1     	| 1   	| 1   	| 1   	| 0-255   	| 1   	|
|--------	|-------	|-----	|-----	|----- |---------	|-----	|
| Name   	| MAGIC 	| LEN 	| SEQ 	| TYPE | PAYLOAD 	| CRC 	|

Explanation of fields:

|                          Byte Index                         	|         C version        	|         Content        	|  Value  	|                                                                            Explanation                                                                            	|
|:-----------------------------------------------------------:	|:------------------------:	|:----------------------:	|:-------:	|:-----------------------------------------------------------------------------------------------------------------------------------------------------------------:	|
| 0                                                           	| uint8_t magic            	| Packet start marker    	| 0x55    	| Protocol-specific start-of-text marker used to indicate the beginning of a new packet. Any system that does not understand protocol version will skip the packet. 	|
| 1                                                          	| uint8_t len              	| Payload length         	| 0 - 255 	| Indicates length of the following payload section (fixed for a particular message).                                                                               	|
| 2                                                           	| uint8_t seq              	| Packet sequence number 	| 0 - 255 	| Used to detect packet loss. Components increment value (mod 256) for each message sent.
| 3                                                           	| uint8_t mstype              	| Message Type 	| 0 - 255 	| Message type in payload. Used to decode data back into message object.                                         	|
| For  n-byte payload: n=0 : NA,  n=1 : 4,  n>=2: 4 to (3+n) 	| uint8_t payload[max 255] 	| Payload data           	|         	| Message data. Content depends on message type                                                                                                                     	|
| n+4                                                         	| uint8_t crc         	| Cyclic Redundancy Check               	|         	| CRC-8 for message (excluding  magic byte).                                                                                                                        	|


- The minimum packet length is 5 bytes for acknowledgment packets without payload.
- The maximum packet length is 260 bytes for full payload.
 
##### Cyclic Redundancy Check
Should check which polynomial to use [CRC](http://users.ece.cmu.edu/~koopman/pubs/KoopmanCRCWebinar9May2012.pdf) 
, following slides have some cool info. Might even be better to use 2byte crc. However 1 byte crc could be implemented with a lookup-table. Which is fast.  (I think that for our max data size 260bytes, we would need a 12-bit CRC to atleast ensure some proper error detection, would have to check)

#### Payload Format

CP16 does not include information about the message structure in the payload itself (in order to reduce overhead)! Instead the sender and receiver must share a common understanding of the meaning, order and size of message fields in the over-the-wire format.

Messages are encoded within the CP16 packet:

 - The _mstype_ (message type field identifies the specific message encoded in the packet.
- The _payload_ field contains the message data.
- - CP16 reorders the message fields in the payload for over-the-wire transmission (from the order in memory).

- The _len_ field contains the length of the payload data.
 - CP16 does NOT truncate zero bytes at the end of the payload. 
 


##### Field Reordering

Message payload fields are reordered for transmission as follows:
Fields are sorted according to their native data size:

	(8 bytes)  (u)int64_t, 
	(4 bytes)  (u)int32_t, 
	(2 bytes)  (u)int16_t 
	(1 byte)  (u)int8_t, char (1)
	
 If two fields have the same length, their order is preserved as it was present before the data field size ordering
Arrays are handled based on the data type they use, not based on the total array size.
 
#### Message Types
|      ID     	|             Name            	| Explanation 	|
|:-----------:	|:---------------------------:	|:-----------:	|
| 0x00 - 0x0F 	| Acknowledgements        	|             	|
| 0x10 - 0x1F 	| Movement Commands           	|             	|
| 0x20 - 0x2F 	| RESERVED	           	|             	|
| 0x30 - 0x3F 	| Telemetry                   	|             	|
| 0x40 - 0x4F 	| Mode Changes / Set Parameters |             	|
| ...         	|                             	|             	|
| 0xE0 - 0xEF 	| RESERVED for status updates 	|             	|
| 0xF0 - 0xFF 	| RESERVED for error codes    	|             	|


##### CP16_CMD_ACK
| BYTE  	|    0    	| 1   	| 2      	| 3 - end     	|
|--------	|:-------:	|-----	|--------	|-------------	|
| NAME   	| MS_TYPE 	| SEQ 	| RESULT 	| DESCRIPTION 	|

- MS_TYPE of the the acknowledged command
- SEQ sequence number of the acknowledged command
- RESULT result of the command (succes, or failure)
- DESCRIPTION discription of the result, why was the command denied (e.g. wrong mode)

##### ACK RESULT values

| Value 	|         Field Name        	| Description                                                                                                	|
|-------	|:-------------------------:	|------------------------------------------------------------------------------------------------------------	|
| 0     	| CP16_CMD_RESULT_OK           	| Command / mission item is ok.                                                                              	|
| 1     	| CP16_CMD_RESULT_FAILED     	| Command is valid, but execution has failed. 	|
| 2     	| CP16_CMD_RESULT_DENIED   	| When the given command is invalid, eg has wrong parameters or quad is in wrong mode                                                  	|
| 3     	| CP16_CMD_RESULT_UNSUPPORTED 	| Command is unknown                                                       	|


##### CP16_CMD_MANUAL_INSTRUCTION (0x10)

| BYTE  	|   0  	| 1    	| 2     	| 3   	|
|--------	|:----:	|------	|-------	|-----	|
| NAME   	| LIFT 	| ROLL 	| PITCH 	| YAW 	|

##### CP16_TELEMETRY_STATUS (0x30)

| BYTE 	| 0    	|      3-6      	| 7   	| 8     	| 9   	| 10 	| 11 	| 12 	| 13       	| 14   	| 15       	|
|------	|------	|:-------------:	|-----	|-------	|-----	|----	|----	|----	|----------	|------	|----------	|
| NAME 	| TIME 	| ae[0] - ae[3] 	| PHI 	| THETA 	| PSI 	| SP 	| SQ 	| SR 	| BAT_VOLT 	| TEMP 	| PRESSURE 	|

##### CP16_MODE_CHANGE (0x40)

| BYTE  	|   0  	|
|--------	|:----:	|
| NAME   	| MODE 	|





