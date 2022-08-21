/*------------------------------------------------------------
 * controlpacket_tx.c -> Processes sent keys
 *
 * Author - Hanzhang Lin
 *
 * April 2022
 *------------------------------------------------------------
 */



#include <inttypes.h>

#include "control_packet_rx.h"

#include "state/state.h"
#include <inttypes.h>


/**
 * @brief processes the send key
 * 
 * @author Hanzhang Lin
 * 
 * @param c 
 */
void process_key(uint8_t c)
{
	switch (c)
	{
    /*
        exit program: receive ESC
    */
	case 27: 
		state_request_mode(MODE_PANIC); 
        state_request_abort(); 
		break;

    /*
        switch mode
    */
	case '0': 
        state_request_mode(MODE_SAFE);
        break;
	case '1': 
        state_request_mode(MODE_PANIC);
        break;
	case '2': 
        state_request_mode(MODE_MANUAL);
        break;
    case '3':
        state_request_mode(MODE_CALIBRATE);
        break;
    case '4':
        state_request_mode(MODE_YAW);
        break;
    case '5':
        state_request_mode(MODE_FULL);
        break;
    case '6':
        state_request_mode(MODE_RAW);
        break;

        
    /*
        control in yaw or full control mode
    */
	case 'a':    // lift up
		state_set_offset(3,32);
        break;
	case 'z':    // lift down
		state_set_offset(3,-32);
        break;
	case 'D': // left arrow, roll up
		state_set_offset(0,-32);
        break;
	case 'C': // right arrow, roll down
 		state_set_offset(0,32);
        break;
	case 'B': // up arrow, pitch down  
		state_set_offset(1,32);
        break;
	case 'A': // down arrow, pitch up
		state_set_offset(1,-32);
        break;

    /*
        modify control parameter
    */
	case 'u':	 // yaw P up
        YAW_P += 1;
        // TODO ADD LOG_FUNCTION
        // printf("Current P(Yaw) value is switched to %d\n", YAW_P);
        break;

	case 'j':	 // yaw control P down
        YAW_P -= 1;
        if (YAW_P<=1){
            YAW_P = 1;
        }
        // printf("Current P(Yaw) value is switched to %d\n", YAW_P);
        break;
    
	case 'q':    // yaw up
    	state_set_offset(2,32);
        break;
        //yaw_setpoint_offset +=10;
        //break;

	case 'w':    // yaw down
        state_set_offset(2,-32);
        break;
        // yaw_setpoint_offset -=10;
        // break;

	case 'i':	 // roll/pitch P1 up
        FULL_P1 += 1;
        // printf("Current P1(Full) value is switched to %d\n", FULL_P1);
        break;

	case 'k':	 // roll/pitch P1 down
        FULL_P1 -= 1;
        if (FULL_P1<=1){
            FULL_P1 = 1;
        }
        // printf("Current P1(Full) value is switched to %d\n", FULL_P1);
        break;

	case 'o':	 // roll/pitch P2 up
        FULL_P2 += 1;
        // printf("Current P1(Full) value is switched to %d\n", FULL_P1);
        break;

	case 'l':	 // roll/pitch P2 down
        FULL_P2 -= 1;
        if (FULL_P2<=1){
            FULL_P2 = 1;
        }
        // printf("Current P1(Full) value is switched to %d\n", FULL_P1);
        break;
    /*
        modify offset
    */

	default:
        break;

	}
}
