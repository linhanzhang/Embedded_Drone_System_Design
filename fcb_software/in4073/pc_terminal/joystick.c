//#include "pc_terminal.h"
/*------------------------------------------------------------------
 *  Joystick communication
 *
 *  Tianrui Mao
 *
 *  1 May 2022
 *------------------------------------------------------------------
 */
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include "joystick.h"


JoystickData *JoystickData_create() {
	JoystickData *jsdata = malloc(sizeof(JoystickData));
	for (int i = 0; i < AXENUM; i++)
		(jsdata->axis)[i] = 0;
	for (int i = 0; i < BUTTONNUM; i++)
		(jsdata->button)[i] = 0;
	return jsdata;
}

void JoystickData_release(JoystickData *jsdata)
{
	free(jsdata);
}

void JoystickData_init(int *fd, char *path_to_joystick, struct js_event *js)
{
	if ((*fd = open(path_to_joystick, O_RDONLY)) < 0) {
		perror("jserroropen");
		exit(1);
	}
	
	js->time = 0;
	js->value = 0;
	js->type = 0;
	js->number = 0;

	fcntl(*fd, F_SETFL, O_NONBLOCK);
}

//this is for reading the value from joystick
void Joystick_getValue(int *fd, struct js_event *js, JoystickData *jsdat)
{
		/* check up on JS
		 */
		while (read(*fd, js, sizeof(struct js_event)) == 
		       			sizeof(struct js_event))  {


			switch(js->type & ~JS_EVENT_INIT) {
				case JS_EVENT_BUTTON:
					
					jsdat->button[js->number] = js->value;
					break;
				case JS_EVENT_AXIS:
					
					jsdat->axis[js->number] = js->value;
					break;
			}
		}
		
		if (errno != EAGAIN) {
			perror("\njs: error reading (EAGAIN)");
			exit (1);
		}
		

}

void Joystick_print(JoystickData *jsdat){
	printf("%d %d %d %d\n", jsdat->axis[0], jsdat->axis[1], jsdat->axis[2], jsdat->axis[3]);
	printf("%u\n", jsdat->button[0]);
	
}